#include <user/memory.h>
#include <user/syscall.h>
#include <user/types.h>



void* memory_map(u64 length,u8 flags){
	return _syscall_memory_map(length,flags);
}



_Bool memory_unmap(void* address,u64 length){
	return _syscall_memory_unmap(address,length);
}



_Bool memory_stats(memory_stats_t* out){
	return _syscall_memory_stats(out,sizeof(memory_stats_t));
}
