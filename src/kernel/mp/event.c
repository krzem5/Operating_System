#include <kernel/acl/acl.h>
#include <kernel/cpu/cpu.h>
#include <kernel/handle/handle.h>
#include <kernel/handle/handle_list.h>
#include <kernel/lock/rwlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/mp/event.h>
#include <kernel/mp/thread.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/types.h>
#include <kernel/util/spinloop.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "event"



#define EVENT_USER_DISPATCH_FLAG_SET_ACTIVE 1
#define EVENT_USER_DISPATCH_FLAG_DISPATCH_ALL 2



static omm_allocator_t* KERNEL_INIT_WRITE _event_allocator=NULL;
static omm_allocator_t* KERNEL_INIT_WRITE _event_thread_container_allocator=NULL;

KERNEL_PUBLIC handle_type_t KERNEL_INIT_WRITE event_handle_type=0;



static void _event_handle_destructor(handle_t* handle){
	event_t* event=KERNEL_CONTAINEROF(handle,event_t,handle);
	if (!event->is_deleted){
		panic("_event_handle_destructor: unreferenced event not deleted");
	}
	omm_dealloc(_event_allocator,event);
}



static bool _await_event(thread_t* thread,event_t* event,u32 index){
	rwlock_acquire_write(&(event->lock));
	if (event->is_deleted){
		WARN("Awaiting deleted event; possibly wake up thread");
		rwlock_release_write(&(event->lock));
		return 0;
	}
	if (event->is_active){
		rwlock_release_write(&(event->lock));
		return 1;
	}
	rwlock_acquire_write(&(thread->lock));
	event_thread_container_t* container=omm_alloc(_event_thread_container_allocator);
	container->thread=thread->handle.rb_node.key;
	container->prev=event->tail;
	container->next=NULL;
	container->sequence_id=thread->event_sequence_id;
	container->index=index;
	if (event->tail){
		event->tail->next=container;
	}
	else{
		event->head=container;
	}
	event->tail=container;
	rwlock_release_write(&(thread->lock));
	rwlock_release_write(&(event->lock));
	return 0;
}



KERNEL_EARLY_EARLY_INIT(){
	LOG("Initializing event allocator...");
	_event_allocator=omm_init("kernel.event",sizeof(event_t),8,4);
	rwlock_init(&(_event_allocator->lock));
	_event_thread_container_allocator=omm_init("kernel.event.thread_container",sizeof(event_thread_container_t),8,4);
	rwlock_init(&(_event_thread_container_allocator->lock));
	event_handle_type=handle_alloc("kernel.event",_event_handle_destructor);
}



KERNEL_PUBLIC event_t* event_create(const char* name){
	event_t* out=omm_alloc(_event_allocator);
	out->name=name;
	handle_new(event_handle_type,&(out->handle));
	out->handle.acl=acl_create();
	if (CPU_HEADER_DATA->current_thread){
		acl_set(out->handle.acl,THREAD_DATA->process,0,EVENT_ACL_FLAG_DISPATCH|EVENT_ACL_FLAG_DELETE);
	}
	rwlock_init(&(out->lock));
	out->is_active=0;
	out->is_deleted=0;
	out->head=NULL;
	out->tail=NULL;
	return out;
}



KERNEL_PUBLIC void event_delete(event_t* event){
	if (CPU_HEADER_DATA->current_thread&&!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DELETE)){
		return;
	}
	rwlock_acquire_write(&(event->lock));
	if (event->is_deleted){
		rwlock_release_write(&(event->lock));
		return;
	}
	event->is_deleted=1;
	while (event->head){
		WARN("Event deleted; update thread and possibly wake it up with event_wakeup_index=EVENT_INDEX_CANCELLED");
		event_thread_container_t* container=event->head;
		event->head=container->next;
		omm_dealloc(_event_thread_container_allocator,container);
	}
	rwlock_release_write(&(event->lock));
	handle_release(&(event->handle));
}



KERNEL_PUBLIC void event_dispatch(event_t* event,u32 flags){
	if (!(flags&EVENT_DISPATCH_FLAG_BYPASS_ACL)&&CPU_HEADER_DATA->current_thread&&!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DISPATCH)){
		return;
	}
	rwlock_acquire_write(&(event->lock));
	if (event->is_deleted){
		rwlock_release_write(&(event->lock));
		return;
	}
	if (flags&EVENT_DISPATCH_FLAG_SET_ACTIVE){
		event->is_active=1;
	}
	while (event->head){
		event_thread_container_t* container=event->head;
		event->head=container->next;
		u64 sequence_id=container->sequence_id;
		u64 index=container->index;
		handle_t* thread_handle=handle_lookup_and_acquire(container->thread,thread_handle_type);
		omm_dealloc(_event_thread_container_allocator,container);
		if (!thread_handle){
			continue;
		}
		thread_t* thread=KERNEL_CONTAINEROF(thread_handle,thread_t,handle);
		if (thread->state!=THREAD_STATE_TYPE_AWAITING_EVENT||thread->event_sequence_id!=sequence_id){
			handle_release(thread_handle);
			continue;
		}
		rwlock_acquire_write(&(thread->lock));
		thread->state=THREAD_STATE_TYPE_NONE;
		thread->event_sequence_id++;
		thread->event_wakeup_index=index;
		rwlock_release_write(&(thread->lock));
		SPINLOOP(thread->reg_state.reg_state_not_present);
		scheduler_enqueue_thread(thread);
		handle_release(thread_handle);
		if (!(flags&EVENT_DISPATCH_FLAG_DISPATCH_ALL)){
			break;
		}
	}
	if (!event->head){
		event->tail=NULL;
	}
	rwlock_release_write(&(event->lock));
}



KERNEL_PUBLIC void event_await(event_t* event,bool is_io_wait){
	THREAD_DATA->scheduler_io_yield=is_io_wait;
	event_await_multiple(&event,1);
	THREAD_DATA->scheduler_io_yield=0;
}



KERNEL_PUBLIC u32 event_await_multiple(event_t*const* events,u32 count){
	if (!count||!CPU_HEADER_DATA->current_thread){
		return 0;
	}
	thread_t* thread=CPU_HEADER_DATA->current_thread;
	scheduler_pause();
	rwlock_acquire_write(&(thread->lock));
	thread->state=THREAD_STATE_TYPE_AWAITING_EVENT;
	thread->reg_state.reg_state_not_present=1;
	rwlock_release_write(&(thread->lock));
	for (u32 i=0;i<count;i++){
		if (!_await_event(thread,events[i],i)){
			continue;
		}
		rwlock_acquire_write(&(thread->lock));
		thread->state=THREAD_STATE_TYPE_RUNNING;
		thread->event_wakeup_index=i;
		rwlock_release_write(&(thread->lock));
		scheduler_resume(1);
		return i;
	}
	scheduler_yield();
	return thread->event_wakeup_index;
}



KERNEL_PUBLIC u32 event_await_multiple_handles(const handle_id_t* handles,u32 count){
	if (!count||!CPU_HEADER_DATA->current_thread){
		return 0;
	}
	thread_t* thread=CPU_HEADER_DATA->current_thread;
	scheduler_pause();
	rwlock_acquire_write(&(thread->lock));
	thread->state=THREAD_STATE_TYPE_AWAITING_EVENT;
	thread->reg_state.reg_state_not_present=1;
	rwlock_release_write(&(thread->lock));
	for (u32 i=0;i<count;i++){
		handle_t* handle_event=handle_lookup_and_acquire(handles[i],event_handle_type);
		if (!handle_event){
			continue;
		}
		bool is_active=_await_event(thread,KERNEL_CONTAINEROF(handle_event,event_t,handle),i);
		handle_release(handle_event);
		if (is_active){
			rwlock_acquire_write(&(thread->lock));
			thread->state=THREAD_STATE_TYPE_RUNNING;
			thread->event_wakeup_index=i;
			rwlock_release_write(&(thread->lock));
			scheduler_resume(1);
			return i;
		}
	}
	scheduler_yield();
	return thread->event_wakeup_index;
}



KERNEL_PUBLIC void event_set_active(event_t* event,bool is_active,bool bypass_acl){
	if (!bypass_acl&&CPU_HEADER_DATA->current_thread&&!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DISPATCH)){
		return;
	}
	rwlock_acquire_write(&(event->lock));
	if (!event->is_deleted){
		event->is_active=is_active;
	}
	rwlock_release_write(&(event->lock));
}



void event_await_thread_irq(thread_t* thread,event_t* event){
	rwlock_acquire_write(&(thread->lock));
	thread->state=THREAD_STATE_TYPE_AWAITING_EVENT;
	thread->reg_state.reg_state_not_present=1;
	rwlock_release_write(&(thread->lock));
	if (_await_event(thread,event,0)){
		panic("event_await_thread_irq: event already active");
	}
}



error_t syscall_event_create(u32 is_active){
	event_t* event=event_create("user");
	handle_list_push(&(THREAD_DATA->process->handle_list),&(event->handle));
	if (is_active){
		event_set_active(event,is_active,0);
	}
	return event->handle.rb_node.key;
}



error_t syscall_event_delete(handle_id_t event_handle){
	handle_t* handle=handle_lookup_and_acquire(event_handle,event_handle_type);
	if (!handle){
		return ERROR_INVALID_HANDLE;
	}
	event_t* event=KERNEL_CONTAINEROF(handle,event_t,handle);
	if (!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DELETE)){
		handle_release(handle);
		return ERROR_DENIED;
	}
	handle_list_pop(&(THREAD_DATA->process->handle_list),&(event->handle));
	event_delete(event);
	handle_release(handle);
	return ERROR_OK;
}



error_t syscall_event_dispatch(handle_id_t event_handle,u32 dispatch_flags){
	handle_t* handle=handle_lookup_and_acquire(event_handle,event_handle_type);
	if (!handle){
		return ERROR_INVALID_HANDLE;
	}
	event_t* event=KERNEL_CONTAINEROF(handle,event_t,handle);
	if (!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DISPATCH)){
		handle_release(handle);
		return ERROR_DENIED;
	}
	u32 flags=0;
	if (dispatch_flags&EVENT_USER_DISPATCH_FLAG_SET_ACTIVE){
		flags|=EVENT_DISPATCH_FLAG_SET_ACTIVE;
	}
	if (dispatch_flags&EVENT_USER_DISPATCH_FLAG_DISPATCH_ALL){
		flags|=EVENT_DISPATCH_FLAG_DISPATCH_ALL;
	}
	event_dispatch(event,flags);
	handle_release(handle);
	return ERROR_OK;
}



error_t syscall_event_set_active(handle_id_t event_handle,u32 is_active){
	handle_t* handle=handle_lookup_and_acquire(event_handle,event_handle_type);
	if (!handle){
		return ERROR_INVALID_HANDLE;
	}
	event_t* event=KERNEL_CONTAINEROF(handle,event_t,handle);
	if (!(acl_get(event->handle.acl,THREAD_DATA->process)&EVENT_ACL_FLAG_DISPATCH)){
		handle_release(handle);
		return ERROR_DENIED;
	}
	event_set_active(event,is_active,0);
	handle_release(handle);
	return ERROR_OK;
}
