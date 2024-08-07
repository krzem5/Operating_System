#include <kernel/lock/rwlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/shutdown/shutdown.h>
#include <kernel/syscall/syscall.h>
#include <kernel/types.h>
#define KERNEL_LOG_NAME "shutdown"



#define USER_SHUTDOWN_FLAG_RESTART 1



static omm_allocator_t* KERNEL_INIT_WRITE _shutdown_function_allocator=NULL;
static shutdown_function_t* _shutdown_root_function=NULL;
static rwlock_t _shutdown_function_lock;



KERNEL_INIT(){
	_shutdown_function_allocator=omm_init("kernel.shutdown.function",sizeof(shutdown_function_t),8,1);
	rwlock_init(&_shutdown_function_lock);
}



KERNEL_PUBLIC void KERNEL_NORETURN shutdown(u32 flags){
	if (!(flags&SHUTDOWN_FLAG_NO_CLEANUP)){
		// dispatch cleanup callbacks
	}
	scheduler_pause();
	if (flags&SHUTDOWN_FLAG_RESTART){
		_shutdown_restart();
	}
	for (const shutdown_function_t* function=_shutdown_root_function;function;function=function->next){
		if (function->is_high_priority){
			function->callback();
		}
	}
	for (const shutdown_function_t* function=_shutdown_root_function;function;function=function->next){
		if (!function->is_high_priority){
			function->callback();
		}
	}
	scheduler_task_wait_loop();
}



KERNEL_PUBLIC void shutdown_register_shutdown_function(shutdown_function_callback_t callback,bool is_high_priority){
	rwlock_acquire_write(&_shutdown_function_lock);
	shutdown_function_t* function=omm_alloc(_shutdown_function_allocator);
	function->next=_shutdown_root_function;
	function->callback=callback;
	function->is_high_priority=is_high_priority;
	_shutdown_root_function=function;
	rwlock_release_write(&_shutdown_function_lock);
}



void KERNEL_NORETURN syscall_system_shutdown(u32 flags){
	shutdown(((flags&USER_SHUTDOWN_FLAG_RESTART)?SHUTDOWN_FLAG_RESTART:0));
}
