#include <kernel/cpu/cpu.h>
#include <kernel/cpu/local.h>
#include <kernel/kernel.h>
#include <kernel/log/log.h>
#include <kernel/memory/kmm.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "cpulocal"



void cpu_local_init(void){
	LOG("Allocating cpu-local data...");
	for (const cpu_local_data_descriptor_t*const* descriptor=(void*)kernel_get_cpu_local_start();(u64)descriptor<kernel_get_cpu_local_end();descriptor++){
		if (*descriptor){
			u32 size=(*descriptor)->size*cpu_count;
			void* data=kmm_alloc(size);
			memset(data,0,size);
			*((*descriptor)->var)=data;
		}
	}
}
