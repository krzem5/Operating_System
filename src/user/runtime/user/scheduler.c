#include <user/scheduler.h>
#include <user/syscall.h>



_Bool scheduler_get_stats(u32 cpu_index,scheduler_stats_t* out){
	return _syscall_scheduler_get_stats(cpu_index,out,sizeof(scheduler_stats_t));
}



_Bool scheduler_get_timers(u32 cpu_index,scheduler_timers_t* out){
	return _syscall_scheduler_get_timers(cpu_index,out,sizeof(scheduler_timers_t));
}