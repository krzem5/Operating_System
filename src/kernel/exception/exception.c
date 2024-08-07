#include <kernel/exception/exception.h>
#include <kernel/lock/profiling.h>
#include <kernel/log/log.h>
#include <kernel/mp/thread.h>
#include <kernel/mp/thread.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "exception"



void exception_unwind(void){
	for (exception_unwind_frame_t* frame=THREAD_DATA->exception_unwind_frame;frame;frame=frame->next){
		frame->callback(frame->args);
	}
	lock_profiling_assert_empty(THREAD_DATA->header.current_thread);
	THREAD_DATA->exception_unwind_frame=NULL;
}



KERNEL_PUBLIC void _exception_push_unwind_frame(exception_unwind_frame_t* frame){
	frame->next=THREAD_DATA->exception_unwind_frame;
	THREAD_DATA->exception_unwind_frame=frame;
}



KERNEL_PUBLIC void _exception_pop_unwind_frame(exception_unwind_frame_t* frame){
	if (THREAD_DATA->exception_unwind_frame!=frame){
		panic("Broken exception chain");
	}
	THREAD_DATA->exception_unwind_frame=frame->next;
}



void _exception_signal_interrupt_handler(u64 error){
	exception_unwind();
	if (THREAD_DATA->exception_is_user){
		_exception_user_return(error);
	}
	thread_terminate((void*)error);
}
