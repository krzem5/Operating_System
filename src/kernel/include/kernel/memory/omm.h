#ifndef _KERNEL_MEMORY_OMM_H_
#define _KERNEL_MEMORY_OMM_H_ 1
#include <kernel/handle/handle.h>
#include <kernel/lock/rwlock.h>
#include <kernel/types.h>



typedef struct _OMM_OBJECT{
	struct _OMM_OBJECT* next;
} omm_object_t;



typedef struct _OMM_PAGE_HEADER{
	struct _OMM_PAGE_HEADER* prev;
	struct _OMM_PAGE_HEADER* next;
	omm_object_t* head;
	u32 object_size;
	u32 used_count;
} omm_page_header_t;



typedef struct _OMM_ALLOCATOR{
	const char* name;
	handle_t handle;
	rwlock_t lock;
	u32 object_size;
	u32 alignment;
	u32 page_count;
	u32 max_used_count;
	omm_page_header_t* page_free_head;
	omm_page_header_t* page_used_head;
	omm_page_header_t* page_full_head;
	KERNEL_ATOMIC u64 allocation_count;
	KERNEL_ATOMIC u64 deallocation_count;
} omm_allocator_t;



extern handle_type_t omm_handle_type;



void omm_init_self(void);



void omm_init_handle_type(omm_allocator_t* handle_allocator);



omm_allocator_t* omm_init(const char* name,u64 object_size,u64 alignment,u64 page_count);



void omm_deinit(omm_allocator_t* allocator);



void* omm_alloc(omm_allocator_t* allocator);



void omm_dealloc(omm_allocator_t* allocator,void* object);



#endif
