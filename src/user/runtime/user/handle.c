#include <user/handle.h>
#include <user/syscall.h>



handle_type_t handle_get_type_count(void){
	return _syscall_handle_get_type_count();
}



_Bool handle_get_type_data(handle_type_t handle_type,handle_type_data_t* out){
	return _syscall_handle_get_type(handle_type,out,sizeof(handle_type_data_t));
}