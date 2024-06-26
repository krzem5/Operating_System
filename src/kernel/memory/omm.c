#include <kernel/handle/handle.h>
#include <kernel/lock/rwlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "omm"



static pmm_counter_descriptor_t KERNEL_EARLY_WRITE _omm_pmm_counter_static=_PMM_COUNTER_INIT_STRUCT("kernel.omm");
static pmm_counter_descriptor_t* KERNEL_INIT_WRITE _omm_pmm_counter=&_omm_pmm_counter_static;
static omm_allocator_t* KERNEL_INIT_WRITE _omm_self_allocator=NULL;

KERNEL_PUBLIC handle_type_t KERNEL_INIT_WRITE omm_handle_type=0;



static void _init_allocator(const char* name,u64 object_size,u64 alignment,u64 page_count,omm_allocator_t* out){
	if (object_size<sizeof(omm_object_t)){
		object_size=sizeof(omm_object_t);
	}
	if (alignment&(alignment-1)){
		panic("omm_init: alignment must be a power of 2");
	}
	if (page_count&(page_count-1)){
		panic("omm_init: page_count must be a power of 2");
	}
	out->name=name;
	rwlock_init(&(out->lock));
	out->object_size=(object_size+alignment-1)&(-alignment);
	out->alignment=alignment;
	out->page_count=page_count;
	out->max_used_count=((page_count<<PAGE_SIZE_SHIFT)-((sizeof(omm_page_header_t)+alignment-1)&(-alignment)))/out->object_size;
	out->page_free_head=NULL;
	out->page_used_head=NULL;
	out->page_full_head=NULL;
	out->allocation_count=0;
	out->deallocation_count=0;
}



static void _allocator_add_page(omm_page_header_t** list_head,omm_page_header_t* page){
	page->prev=NULL;
	page->next=*list_head;
	if (*list_head){
		(*list_head)->prev=page;
	}
	*list_head=page;
}



static void _allocator_remove_page(omm_page_header_t** list_head,omm_page_header_t* page){
	if (page->prev){
		page->prev->next=page->next;
	}
	else{
		*list_head=page->next;
	}
	if (page->next){
		page->next->prev=page->prev;
	}
}



KERNEL_EARLY_EARLY_INIT(){
	_omm_pmm_counter=pmm_alloc_counter("kernel.omm");
	_omm_pmm_counter->count=_omm_pmm_counter_static.count;
}



void KERNEL_EARLY_EXEC omm_init_self(void){
	omm_allocator_t _tmp_allocator;
	_init_allocator("kernel.omm",sizeof(omm_allocator_t),8,2,&_tmp_allocator);
	_omm_self_allocator=omm_alloc(&_tmp_allocator);
	*_omm_self_allocator=_tmp_allocator;
}



void KERNEL_EARLY_EXEC omm_init_handle_type(omm_allocator_t* handle_allocator){
	omm_handle_type=handle_alloc("kernel.omm.allocator",0,NULL);
	handle_new(omm_handle_type,&(_omm_self_allocator->handle));
	handle_new(omm_handle_type,&(handle_allocator->handle));
}



KERNEL_PUBLIC omm_allocator_t* omm_init(const char* name,u64 object_size,u64 alignment,u64 page_count){
	if (!_omm_self_allocator){
		panic("omm allocator not initialized yet");
	}
	omm_allocator_t* out=omm_alloc(_omm_self_allocator);
	_init_allocator(name,object_size,alignment,page_count,out);
	if (omm_handle_type){
		handle_new(omm_handle_type,&(out->handle));
	}
	return out;
}



KERNEL_PUBLIC void omm_deinit(omm_allocator_t* allocator){
	handle_destroy(&(allocator->handle));
	omm_dealloc(_omm_self_allocator,allocator);
}



KERNEL_PUBLIC void* omm_alloc(omm_allocator_t* allocator){
	rwlock_acquire_write(&(allocator->lock));
	omm_page_header_t* page=(allocator->page_used_head?allocator->page_used_head:allocator->page_free_head);
	if (!page){
		u64 page_address=pmm_alloc(allocator->page_count,_omm_pmm_counter,0)+VMM_HIGHER_HALF_ADDRESS_OFFSET;
		omm_object_t* head=NULL;
		for (u64 i=(sizeof(omm_page_header_t)+allocator->alignment-1)&(-((u64)(allocator->alignment)));i+allocator->object_size<=(allocator->page_count<<PAGE_SIZE_SHIFT);i+=allocator->object_size){
			omm_object_t* object=(void*)(page_address+i);
			object->next=head;
			head=object;
		}
		page=(void*)page_address;
		page->head=head;
		page->object_size=allocator->object_size;
		page->used_count=0;
		_allocator_add_page(&(allocator->page_free_head),page);
	}
	void* out=page->head;
	page->head=page->head->next;
	page->used_count++;
	if (page->used_count==(allocator->max_used_count>>1)){
		_allocator_remove_page(&(allocator->page_free_head),page);
		_allocator_add_page(&(allocator->page_used_head),page);
	}
	else if (page->used_count==allocator->max_used_count){
		_allocator_remove_page(&(allocator->page_used_head),page);
		_allocator_add_page(&(allocator->page_full_head),page);
	}
	allocator->allocation_count++;
	rwlock_release_write(&(allocator->lock));
	return out;
}



KERNEL_PUBLIC void omm_dealloc(omm_allocator_t* allocator,void* object){
	rwlock_acquire_write(&(allocator->lock));
	omm_page_header_t* page=(void*)(((u64)object)&(-(((u64)(allocator->page_count))<<PAGE_SIZE_SHIFT)));
	if (page->object_size!=allocator->object_size){
		panic("omm_dealloc: wrong allocator");
	}
	omm_object_t* head=object;
	head->next=page->head;
	page->head=head;
	if (page->used_count==allocator->max_used_count){
		_allocator_remove_page(&(allocator->page_full_head),page);
		_allocator_add_page(&(allocator->page_used_head),page);
	}
	else if (page->used_count==(allocator->max_used_count>>1)){
		_allocator_remove_page(&(allocator->page_used_head),page);
		_allocator_add_page(&(allocator->page_free_head),page);
	}
	page->used_count--;
	u64 delete_page_count=0;
	if (!page->used_count){
		_allocator_remove_page(&(allocator->page_free_head),page);
		delete_page_count=allocator->page_count;
	}
	allocator->deallocation_count++;
	rwlock_release_write(&(allocator->lock));
	if (delete_page_count){
		pmm_dealloc(((u64)page)-VMM_HIGHER_HALF_ADDRESS_OFFSET,delete_page_count,_omm_pmm_counter);
	}
}
