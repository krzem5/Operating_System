#include <kernel/error/error.h>
#include <kernel/id/flags.h>
#include <kernel/id/group.h>
#include <kernel/lock/rwlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/smm.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/syscall/syscall.h>
#include <kernel/tree/rb_tree.h>
#include <kernel/types.h>
#include <kernel/util/string.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "gid"



typedef struct _GID_DATA{
	rb_tree_node_t rb_node;
	string_t* name;
	id_flags_t flags;
} gid_data_t;



static omm_allocator_t* KERNEL_INIT_WRITE _gid_data_allocator=NULL;
static rb_tree_t _gid_tree;
static rwlock_t _gid_global_lock;



KERNEL_EARLY_INIT(){
	LOG("Initializing group tree...");
	_gid_data_allocator=omm_init("kernel.gid.data",sizeof(gid_data_t),8,1);
	rwlock_init(&(_gid_data_allocator->lock));
	rwlock_init(&_gid_global_lock);
	rb_tree_init(&_gid_tree);
	INFO("Creating root group...");
	if (gid_create(0,"root")!=ERROR_OK){
		panic("Unable to create root group");
	}
}



KERNEL_PUBLIC error_t gid_create(gid_t gid,const char* name){
	rwlock_acquire_write(&_gid_global_lock);
	if (rb_tree_lookup_node(&_gid_tree,gid)){
		rwlock_release_write(&_gid_global_lock);
		return ERROR_ALREADY_PRESENT;
	}
	gid_data_t* gid_data=omm_alloc(_gid_data_allocator);
	gid_data->rb_node.key=gid;
	gid_data->name=smm_alloc(name,0);
	gid_data->flags=0;
	rb_tree_insert_node(&_gid_tree,&(gid_data->rb_node));
	rwlock_release_write(&_gid_global_lock);
	return ERROR_OK;
}



KERNEL_PUBLIC error_t gid_delete(gid_t gid){
	rwlock_acquire_write(&_gid_global_lock);
	gid_data_t* gid_data=(gid_data_t*)rb_tree_lookup_node(&_gid_tree,gid);
	if (!gid_data){
		rwlock_release_write(&_gid_global_lock);
		return ERROR_NOT_FOUND;
	}
	smm_dealloc(gid_data->name);
	rb_tree_remove_node(&_gid_tree,&(gid_data->rb_node));
	omm_dealloc(_gid_data_allocator,gid_data);
	rwlock_release_write(&_gid_global_lock);
	return ERROR_OK;
}



KERNEL_PUBLIC error_t gid_get_name(gid_t gid,char* buffer,u32 buffer_length){
	if (!buffer_length){
		return ERROR_NO_SPACE;
	}
	rwlock_acquire_read(&_gid_global_lock);
	gid_data_t* gid_data=(gid_data_t*)rb_tree_lookup_node(&_gid_tree,gid);
	if (!gid_data){
		rwlock_release_read(&_gid_global_lock);
		return ERROR_NOT_FOUND;
	}
	str_copy(gid_data->name->data,buffer,buffer_length);
	rwlock_release_read(&_gid_global_lock);
	return ERROR_OK;
}



KERNEL_PUBLIC id_flags_t gid_get_flags(gid_t gid){
	rwlock_acquire_read(&_gid_global_lock);
	gid_data_t* gid_data=(gid_data_t*)rb_tree_lookup_node(&_gid_tree,gid);
	if (!gid_data){
		rwlock_release_read(&_gid_global_lock);
		return ERROR_NOT_FOUND;
	}
	id_flags_t out=gid_data->flags;
	rwlock_release_read(&_gid_global_lock);
	return out;
}



KERNEL_PUBLIC error_t gid_set_flags(gid_t gid,id_flags_t clear,id_flags_t set){
	rwlock_acquire_read(&_gid_global_lock);
	gid_data_t* gid_data=(gid_data_t*)rb_tree_lookup_node(&_gid_tree,gid);
	if (!gid_data){
		rwlock_release_read(&_gid_global_lock);
		return ERROR_NOT_FOUND;
	}
	gid_data->flags=(gid_data->flags&(~clear))|set;
	rwlock_release_read(&_gid_global_lock);
	return ERROR_OK;
}



error_t syscall_gid_get(void){
	return THREAD_DATA->process->gid;
}



error_t syscall_gid_set(u64 gid){
	if (process_is_root()){
		THREAD_DATA->process->gid=gid;
		return ERROR_OK;
	}
	return ERROR_DENIED;
}



error_t syscall_gid_get_name(u64 gid,KERNEL_USER_POINTER char* buffer,u32 buffer_length){
	if (!buffer_length){
		return ERROR_INVALID_ARGUMENT(2);
	}
	if (buffer_length>syscall_get_user_pointer_max_length((char*)buffer)){
		return ERROR_INVALID_ARGUMENT(1);
	}
	return gid_get_name(gid,(char*)buffer,buffer_length);
}
