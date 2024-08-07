#ifndef _SYS_MP_PROCESS_H_
#define _SYS_MP_PROCESS_H_ 1
#include <sys/fd/fd.h>
#include <sys/id/group.h>
#include <sys/id/user.h>
#include <sys/mp/event.h>
#include <sys/types.h>



#define SYS_PROCESS_START_FLAG_PAUSE_THREAD 1

#define SYS_PROCESS_ACL_FLAG_CREATE_THREAD 1
#define SYS_PROCESS_ACL_FLAG_TERMINATE 2
#define SYS_PROCESS_ACL_FLAG_SWITCH_USER 4



typedef u64 sys_process_t;



typedef struct _SYS_PROCESS_QUERY_RESULT{
	sys_process_t pid;
	sys_process_t ppid;
	char name[256];
	char image[4096];
	sys_uid_t uid;
	sys_gid_t gid;
	char vfs_cwd[4096];
	sys_fd_t fd_stdin;
	sys_fd_t fd_stdout;
	sys_fd_t fd_stderr;
} sys_process_query_result_t;



sys_event_t sys_process_get_termination_event(sys_process_t process);



sys_process_t sys_process_get_handle(void);



sys_process_t __attribute__((access(read_only,1),access(read_only,3,2),access(read_only,4))) sys_process_start(const char* path,u32 argc,const char*const* argv,const char*const* environ,u32 flags,sys_fd_t stdin,sys_fd_t stdout,sys_fd_t stderr);



sys_error_t sys_process_set_cwd(sys_process_t process,sys_fd_t fd);



sys_process_t sys_process_get_parent(sys_process_t process);



sys_error_t sys_process_set_root(sys_process_t process,sys_fd_t fd);



sys_error_t sys_process_get_main_thread(sys_process_t process);



void* sys_process_get_return_value(sys_process_t process);



sys_process_t sys_process_iter_start(void);



sys_process_t sys_process_iter_next(sys_process_t process);



sys_error_t sys_process_query(sys_process_t process,sys_process_query_result_t* out);



#endif
