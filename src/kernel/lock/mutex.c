#include <kernel/lock/mutex.h>
#include <kernel/lock/rwlock.h>
#include <kernel/memory/omm.h>
#include <kernel/mp/event.h>
#include <kernel/types.h>
#include <kernel/util/util.h>



static omm_allocator_t* KERNEL_INIT_WRITE _mutex_allocator=NULL;



KERNEL_EARLY_INIT(){
	_mutex_allocator=omm_init("mutex",sizeof(mutex_t),8,4);
}



mutex_t* mutex_init(void){
	mutex_t* out=omm_alloc(_mutex_allocator);
	rwlock_init(&(out->lock));
	out->holder=NULL;
	out->event=event_create();
	return out;
}



void mutex_deinit(mutex_t* lock){
	if (lock->holder){
		panic("mutex_deinit: lock is held");
	}
	event_delete(lock->event);
	omm_dealloc(_mutex_allocator,lock);
}



void mutex_acquire(mutex_t* lock){
	rwlock_acquire_write(&(lock->lock));
	while (lock->holder){
		rwlock_release_write(&(lock->lock));
		event_await(lock->event,0);
		rwlock_acquire_write(&(lock->lock));
	}
	lock->holder=THREAD_DATA->header.current_thread;
	rwlock_release_write(&(lock->lock));
}



void mutex_release(mutex_t* lock){
	rwlock_acquire_write(&(lock->lock));
	if (lock->holder!=THREAD_DATA->header.current_thread){
		panic("mutex_release: lock released by wrong thread");
	}
	lock->holder=NULL;
	rwlock_release_write(&(lock->lock));
}



bool mutex_is_held(mutex_t* lock){
	return !!lock->holder;
}