#ifndef _KERNEL_SCHEDULER_SCHEDULER_H_
#define _KERNEL_SCHEDULER_SCHEDULER_H_ 1
#include <kernel/isr/isr.h>
#include <kernel/lock/lock.h>
#include <kernel/thread/thread.h>
#include <kernel/types.h>



#define SCHEDULER_PRIORITY_QUEUE_COUNT 3



typedef struct _SCHEDULER_QUEUE{
	lock_t lock;
	thread_t* head;
	thread_t* tail;
} scheduler_queue_t;



typedef struct _SCHEDULER_QUEUES{
	scheduler_queue_t background_queue;
	scheduler_queue_t priority_queues[SCHEDULER_PRIORITY_QUEUE_COUNT];
	scheduler_queue_t realtime_queue;
} scheduler_queues_t;



typedef struct _SCHEDULER{
	thread_t* current_thread;
	u8 priority_timing;
} scheduler_t;



void scheduler_init(void);



scheduler_t* scheduler_new(void);



void KERNEL_NORETURN scheduler_start(void);



void scheduler_isr_handler(isr_state_t* state);



void scheduler_enqueue_thread(thread_t* thread);



void KERNEL_NORETURN scheduler_dequeue(void);



void KERNEL_NORETURN scheduler_task_wait_loop(void);



void scheduler_init_fpu(void* fpu_state);



void scheduler_save_fpu(void* fpu_state);



void scheduler_restore_fpu(void* fpu_state);



#endif
