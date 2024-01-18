#include <sys/error/error.h>
#include <sys/fs/fs.h>
#include <sys/io/io.h>
#include <sys/string/string.h>
#include <sys/types.h>



int main(int argc,const char** argv){
	for (sys_fs_t fs=sys_fs_iter_start();fs;fs=sys_fs_iter_next(fs)){
		sys_fs_data_t data;
		if (SYS_IS_ERROR(sys_fs_get_data(fs,&data))){
			continue;
		}
		sys_io_print("%g: %s\n",data.uuid,data.type);
		if (!sys_string_compare(data.type,"fuse")){
			sys_fs_mount(fs,"/share");
		}
	}
	return 0;
}
