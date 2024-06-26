#include <sys/error/error.h>
#include <sys/mp/event.h>
#include <sys/syscall/kernel_syscalls.h>
#include <sys/types.h>



SYS_PUBLIC sys_event_t sys_event_create(bool is_active){
	return _sys_syscall_event_create(is_active);
}



SYS_PUBLIC sys_error_t sys_event_delete(sys_event_t event){
	return _sys_syscall_event_delete(event);
}



SYS_PUBLIC sys_error_t sys_event_dispatch(sys_event_t event,u32 dispatch_flags){
	return _sys_syscall_event_dispatch(event,dispatch_flags);
}



SYS_PUBLIC sys_error_t sys_event_set_active(sys_event_t event,bool is_active){
	return _sys_syscall_event_set_active(event,is_active);
}
