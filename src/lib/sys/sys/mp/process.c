#include <sys/mp/event.h>
#include <sys/mp/process.h>
#include <sys/syscall/kernel_syscalls.h>
#include <sys/types.h>



typedef struct _SYSCALL_PROCESS_START_EXTRA_DATA{
	u64 stdin;
	u64 stdout;
	u64 stderr;
} syscall_process_start_extra_data_t;



SYS_PUBLIC sys_event_t sys_process_get_termination_event(sys_process_t process){
	return _sys_syscall_process_get_event(process);
}



SYS_PUBLIC sys_process_t sys_process_get_handle(void){
	return _sys_syscall_process_get_pid();
}



SYS_PUBLIC sys_process_t sys_process_start(const char* path,u32 argc,const char*const* argv,const char*const* environ,u32 flags,sys_fd_t stdin,sys_fd_t stdout,sys_fd_t stderr){
	syscall_process_start_extra_data_t extra_data={
		stdin,
		stdout,
		stderr
	};
	return _sys_syscall_process_start(path,argc,argv,environ,flags,&extra_data);
}



SYS_PUBLIC sys_error_t sys_process_set_cwd(sys_process_t process,sys_fd_t fd){
	return _sys_syscall_process_set_cwd(process,fd);
}



SYS_PUBLIC sys_process_t sys_process_get_parent(sys_process_t process){
	return _sys_syscall_process_get_parent(process);
}



SYS_PUBLIC sys_error_t sys_process_set_root(sys_process_t process,sys_fd_t fd){
	return _sys_syscall_process_set_root(process,fd);
}



SYS_PUBLIC sys_error_t sys_process_get_main_thread(sys_process_t process){
	return _sys_syscall_process_get_main_thread(process);
}



SYS_PUBLIC void* sys_process_get_return_value(sys_process_t process){
	return (void*)_sys_syscall_process_get_return_value(process);
}



SYS_PUBLIC sys_process_t sys_process_iter_start(void){
	return _sys_syscall_process_iter(0);
}



SYS_PUBLIC sys_process_t sys_process_iter_next(sys_process_t process){
	return _sys_syscall_process_iter(process);
}



SYS_PUBLIC sys_error_t sys_process_query(sys_process_t process,sys_process_query_result_t* out){
	return _sys_syscall_process_query(process,out,sizeof(sys_process_query_result_t));
}
