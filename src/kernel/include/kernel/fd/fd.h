#ifndef _KERNEL_FD_FD_H_
#define _KERNEL_FD_FD_H_ 1
#include <kernel/vfs/allocator.h>
#include <kernel/types.h>



// Must be a multiple of 64
#define FD_MAX_COUNT 1024



#define FD_ERROR_INVALID_FLAGS -1
#define FD_ERROR_INVALID_FD -2
#define FD_ERROR_INVALID_POINTER -3
#define FD_ERROR_OUT_OF_FDS -4
#define FD_ERROR_NOT_FOUND -5
#define FD_ERROR_UNSUPPORTED_OPERATION -6
#define FD_ERROR_NO_RELATIVE -7
#define FD_ERROR_NOT_EMPTY -8
#define FD_ERROR_DIFFERENT_FS -9
#define FD_ERROR_DIFFERENT_TYPE -10
#define FD_ERROR_NO_SPACE -11

#define FD_FLAG_READ 1
#define FD_FLAG_WRITE 2
#define FD_FLAG_APPEND 4
#define FD_FLAG_CREATE 8
#define FD_FLAG_DIRECTORY 16

#define FD_SEEK_SET 0
#define FD_SEEK_ADD 1
#define FD_SEEK_END 2

#define FD_RELATIVE_PARENT 0
#define FD_RELATIVE_PREV_SIBLING 1
#define FD_RELATIVE_NEXT_SIBLING 2
#define FD_RELATIVE_FIRST_CHILD 3

#define FD_OUT_OF_RANGE(fd) ((fd)<=0||(fd)>FD_MAX_COUNT)



typedef u16 fd_t;



typedef struct _FD_DATA{
	vfs_node_id_t node_id;
	lock_t lock;
	u64 offset;
	u8 flags;
} fd_data_t;



typedef struct _FD_STAT{
	vfs_node_id_t node_id;
	u8 type;
	u8 vfs_index;
	u8 name_length;
	char name[64];
	u64 size;
} fd_stat_t;



extern fd_data_t fd_data[];
extern u16 fd_count;



void fd_clear(void);



int fd_open(fd_t root,const char* path,u32 length,u8 flags);



int fd_close(fd_t fd);



int fd_delete(fd_t fd);



s64 fd_read(fd_t fd,void* buffer,u64 count);



s64 fd_write(fd_t fd,const void* buffer,u64 count);



s64 fd_seek(fd_t fd,u64 offset,u8 type);



int fd_resize(fd_t fd,u64 size);



int fd_absolute_path(fd_t fd,char* buffer,u32 buffer_length);



int fd_stat(fd_t fd,fd_stat_t* out);



int fd_get_relative(fd_t fd,u8 relative,u8 flags);



int fd_move(fd_t fd,fd_t dst_fd);



#endif
