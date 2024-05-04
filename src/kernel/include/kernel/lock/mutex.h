#ifndef _KERNEL_LOCK_MUTEX_H_
#define _KERNEL_LOCK_MUTEX_H_ 1
#include <kernel/lock/rwlock.h>
#include <kernel/mp/event.h>
#include <kernel/mp/thread.h>
#include <kernel/types.h>



typedef struct _MUTEX{
	rwlock_t lock;
	thread_t* holder;
	event_t* event;
} mutex_t;



mutex_t* mutex_init(void);



void mutex_deinit(mutex_t* lock);



void mutex_acquire(mutex_t* lock);



void mutex_release(mutex_t* lock);



bool mutex_is_held(mutex_t* lock);



#endif
