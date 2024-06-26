#ifndef _SYS_MP_PROCESS_GROUP_H_
#define _SYS_MP_PROCESS_GROUP_H_ 1
#include <sys/mp/process.h>
#include <sys/types.h>



typedef u64 sys_process_group_t;



sys_process_group_t sys_process_group_get(sys_process_t process);



sys_process_group_t sys_process_group_set(sys_process_t process,sys_process_group_t process_group);



sys_process_group_t sys_process_group_iter_start(void);



sys_process_group_t sys_process_group_iter_next(sys_process_group_t process_group);



sys_process_t sys_process_group_iter_group_start(sys_process_group_t process_group);



sys_process_t sys_process_group_iter_group_next(sys_process_group_t process_group,sys_process_t process);



#endif
