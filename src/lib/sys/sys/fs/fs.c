#include <sys/error/error.h>
#include <sys/fs/fs.h>
#include <sys/partition/partition.h>
#include <sys/syscall/kernel_syscalls.h>
#include <sys/types.h>



SYS_PUBLIC sys_fs_t sys_fs_iter_start(void){
	return _sys_syscall_fs_get_next(0);
}



SYS_PUBLIC sys_fs_t sys_fs_iter_next(sys_fs_t fs){
	return _sys_syscall_fs_get_next(fs);
}



SYS_PUBLIC sys_error_t sys_fs_get_data(sys_fs_t fs,sys_fs_data_t* out){
	return _sys_syscall_fs_get_data(fs,out,sizeof(sys_fs_data_t));
}



SYS_PUBLIC sys_error_t sys_fs_mount(sys_fs_t fs,const char* path){
	return _sys_syscall_fs_mount(fs,path);
}



SYS_PUBLIC sys_error_t sys_fs_format(sys_partition_t partition,sys_fs_descriptor_t fs_descriptor){
	return _sys_syscall_fs_format(partition,fs_descriptor);
}



SYS_PUBLIC sys_fs_descriptor_t sys_fs_descriptor_iter_start(void){
	return _sys_syscall_fs_descriptor_get_next(0);
}



SYS_PUBLIC sys_fs_descriptor_t sys_fs_descriptor_iter_next(sys_fs_descriptor_t fs_descriptor){
	return _sys_syscall_fs_descriptor_get_next(fs_descriptor);
}



SYS_PUBLIC sys_error_t sys_fs_descriptor_get_data(sys_fs_descriptor_t fs_descriptor,sys_fs_descriptor_data_t* out){
	return _sys_syscall_fs_descriptor_get_data(fs_descriptor,out,sizeof(sys_fs_descriptor_data_t));
}
