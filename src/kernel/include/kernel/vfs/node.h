#ifndef _KERNEL_VFS_NODE_H_
#define _KERNEL_VFS_NODE_H_ 1
#include <kernel/handle/handle.h>
#include <kernel/id/group.h>
#include <kernel/id/user.h>
#include <kernel/lock/rwlock.h>
#include <kernel/memory/smm.h>
#include <kernel/types.h>



#define VFS_NODE_TYPE_MASK 7
#define VFS_NODE_TYPE_UNKNOWN 0
#define VFS_NODE_TYPE_FILE 1
#define VFS_NODE_TYPE_DIRECTORY 2
#define VFS_NODE_TYPE_LINK 3
#define VFS_NODE_TYPE_PIPE 4
#define VFS_NODE_TYPE_SOCKET 5

#define VFS_NODE_FLAG_DIRTY 8
#define VFS_NODE_FLAG_VIRTUAL 16
#define VFS_NODE_FLAG_PERMANENT 32
#define VFS_NODE_FLAG_RESIZE_RELATIVE 64
#define VFS_NODE_FLAG_NONBLOCKING 128
#define VFS_NODE_FLAG_PIPE_PEEK 256
#define VFS_NODE_FLAG_CREATE 512
#define VFS_NODE_FLAG_GROW 1024
#define VFS_NODE_FLAG_TEMPORARY 2048
#define VFS_NODE_FLAG_BYPASS_LOCK 4096

#define VFS_NODE_PERMISSION_SHIFT 13
#define VFS_NODE_PERMISSION_MASK (0x1ff<<VFS_NODE_PERMISSION_SHIFT)

#define VFS_NODE_LOCK_TYPE_NONE 0
#define VFS_NODE_LOCK_TYPE_PROCESS_GROUP 1
#define VFS_NODE_LOCK_TYPE_PROCESS 2
#define VFS_NODE_LOCK_TYPE_THREAD 3
#define VFS_NODE_LOCK_TYPE_INVALID 0xff



typedef struct _VFS_FUNCTIONS{
	struct _VFS_NODE* (*create)(struct _VFS_NODE*,const string_t*,u32);
	void (*delete)(struct _VFS_NODE*);
	struct _VFS_NODE* (*lookup)(struct _VFS_NODE*,const string_t*);
	u64 (*iterate)(struct _VFS_NODE*,u64,string_t**);
	bool (*link)(struct _VFS_NODE*,struct _VFS_NODE*);
	bool (*unlink)(struct _VFS_NODE*,struct _VFS_NODE*);
	u64 (*read)(struct _VFS_NODE*,u64,void*,u64,u32);
	u64 (*write)(struct _VFS_NODE*,u64,const void*,u64,u32);
	u64 (*resize)(struct _VFS_NODE*,s64,u32);
	void (*flush)(struct _VFS_NODE*);
	struct _EVENT* (*event)(struct _VFS_NODE*,bool);
} vfs_functions_t;



typedef struct _VFS_NODE_RELATIVES{
	struct _VFS_NODE* parent;
	struct _VFS_NODE* prev_sibling;
	struct _VFS_NODE* next_sibling;
	struct _VFS_NODE* child;
} vfs_node_relatives_t;



typedef struct _VFS_NODE_LOCK{
	rwlock_t lock;
	u32 type;
	handle_id_t handle;
} vfs_node_lock_t;



typedef struct _VFS_NODE{
	rwlock_t lock;
	u32 flags;
	KERNEL_ATOMIC u64 rc;
	string_t* name;
	vfs_node_relatives_t relatives;
	struct _FILESYSTEM* fs;
	const vfs_functions_t* functions;
	u64 time_access;
	u64 time_modify;
	u64 time_change;
	u64 time_birth;
	gid_t gid;
	uid_t uid;
	vfs_node_lock_t io_lock;
} vfs_node_t;



vfs_node_t* vfs_node_create(struct _FILESYSTEM* fs,vfs_node_t* parent,const string_t* name,u32 flags);



vfs_node_t* vfs_node_create_virtual(vfs_node_t* parent,const vfs_functions_t* functions,const string_t* name);



void vfs_node_delete(vfs_node_t* node);



vfs_node_t* vfs_node_lookup(vfs_node_t* node,const string_t* name);



u64 vfs_node_iterate(vfs_node_t* node,u64 pointer,string_t** out);



bool vfs_node_link(vfs_node_t* node,vfs_node_t* parent);



bool vfs_node_unlink(vfs_node_t* node);



u64 vfs_node_read(vfs_node_t* node,u64 offset,void* buffer,u64 size,u32 flags);



u64 vfs_node_write(vfs_node_t* node,u64 offset,const void* buffer,u64 size,u32 flags);



u64 vfs_node_resize(vfs_node_t* node,s64 offset,u32 flags);



void vfs_node_flush(vfs_node_t* node);



struct _EVENT* vfs_node_get_event(vfs_node_t* node,bool write);



void vfs_node_attach_child(vfs_node_t* node,vfs_node_t* child);



void vfs_node_dettach_child(vfs_node_t* node);



void _vfs_node_process_unref(vfs_node_t* node);



static KERNEL_INLINE void vfs_node_ref(vfs_node_t* node){
	node->rc++;
}



static KERNEL_INLINE void vfs_node_unref(vfs_node_t* node){
	node->rc--;
	if (!node->rc){
		_vfs_node_process_unref(node);
	}
}



#endif
