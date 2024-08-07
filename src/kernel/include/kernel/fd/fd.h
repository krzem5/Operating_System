#ifndef _KERNEL_FD_FD_H_
#define _KERNEL_FD_FD_H_ 1
#include <kernel/acl/acl.h>
#include <kernel/handle/handle.h>
#include <kernel/lock/mutex.h>
#include <kernel/memory/smm.h>
#include <kernel/mp/process.h>
#include <kernel/types.h>
#include <kernel/vfs/node.h>



#define FD_FLAG_READ 1
#define FD_FLAG_WRITE 2
#define FD_FLAG_APPEND 4
#define FD_FLAG_CREATE 8
#define FD_FLAG_DIRECTORY 16
#define FD_FLAG_IGNORE_LINKS 32
#define FD_FLAG_NONBLOCKING 64
#define FD_FLAG_PIPE_PEEK 128
#define FD_FLAG_DELETE_ON_EXIT 256
#define FD_FLAG_EXCLUSIVE_CREATE 512
#define FD_FLAG_LINK 1024
#define FD_FLAG_CLOSE_PIPE 2048
#define FD_FLAG_FIND_LINKS 4096

#define FD_SEEK_SET 0
#define FD_SEEK_ADD 1
#define FD_SEEK_END 2

#define FD_STAT_FLAG_VIRTUAL 1

#define FD_ACL_FLAG_STAT 1
#define FD_ACL_FLAG_DUP 2
#define FD_ACL_FLAG_IO 4
#define FD_ACL_FLAG_CLOSE 8

#define FD_ITERATOR_ACL_FLAG_ACCESS 1

#define FD_DUP_CWD 0x7ffffffffffffffcull
#define FD_DUP_STDIN 0x7ffffffffffffffdull
#define FD_DUP_STDOUT 0x7ffffffffffffffeull
#define FD_DUP_STDERR 0x7fffffffffffffffull



typedef struct _FD{
	handle_t handle;
	mutex_t* lock;
	vfs_node_t* node;
	u64 offset;
	u32 flags;
} fd_t;



typedef struct _FD_STAT{
	u8 type;
	u8 flags;
	u16 permissions;
	u8 name_length;
	u64 fs_handle;
	u64 size;
	u64 time_access;
	u64 time_modify;
	u64 time_change;
	u64 time_birth;
	u32 gid;
	u32 uid;
	u64 lock_handle;
	char name[256];
} fd_stat_t;



typedef struct _FD_ITERATOR{
	handle_t handle;
	mutex_t* lock;
	vfs_node_t* node;
	u64 pointer;
	string_t* current_name;
} fd_iterator_t;



error_t fd_from_node(vfs_node_t* node,u32 flags);



vfs_node_t* fd_get_node(handle_id_t fd,u64* acl);



void fd_allow_dup(handle_id_t fd,process_t* process);



void fd_ref(handle_id_t fd);



void fd_unref(handle_id_t fd);



#endif
