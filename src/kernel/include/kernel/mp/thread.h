#ifndef _KERNEL_MP_THREAD_H_
#define _KERNEL_MP_THREAD_H_ 1
#include <kernel/mp/_mp_types.h>
#include <kernel/types.h>



#define THREAD_STATE_TYPE_NONE 0
#define THREAD_STATE_TYPE_QUEUED 1
#define THREAD_STATE_TYPE_RUNNING 2
#define THREAD_STATE_TYPE_AWAITING_EVENT 3
#define THREAD_STATE_TYPE_TERMINATED 255

#define THREAD_DATA ((volatile __seg_gs thread_t*)NULL)



extern handle_type_t HANDLE_TYPE_THREAD;



thread_t* thread_new_user(process_t* process,u64 rip,u64 stack_size);



thread_t* thread_new_kernel(process_t* process,u64 rip,u64 stack_size,u8 arg_count,...);



void thread_delete(thread_t* thread);



void KERNEL_NORETURN thread_terminate(void);



void thread_await_event(event_t* event);



void KERNEL_NORETURN _thread_bootstrap_kernel_thread(void);



#endif
