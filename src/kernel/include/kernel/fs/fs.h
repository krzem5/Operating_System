#ifndef _KERNEL_FS_FS_H_
#define _KERNEL_FS_FS_H_ 1
#include <kernel/drive/drive.h>
#include <kernel/fs/node_allocator.h>
#include <kernel/fs/partition.h>
#include <kernel/lock/lock.h>
#include <kernel/types.h>



#define FS_FILE_SYSTEM_CONFIG_FLAG_ALIGNED_IO 1

#define FS_FILE_SYSTEM_FLAG_BOOT 1
#define FS_FILE_SYSTEM_FLAG_HALF_INSTALLED 2
#define FS_FILE_SYSTEM_FLAG_PREVIOUS_BOOT 4

#define FS_MAX_FILE_SYSTEMS 64
#define FS_INVALID_FILE_SYSTEM_INDEX FS_MAX_FILE_SYSTEMS

#define FS_NODE_RELATIVE_PARENT 0
#define FS_NODE_RELATIVE_PREV_SIBLING 1
#define FS_NODE_RELATIVE_NEXT_SIBLING 2
#define FS_NODE_RELATIVE_FIRST_CHILD 3



struct _FS_FILE_SYSTEM;



typedef struct _FS_FILE_SYSTEM_CONFIG{
	u8 node_size;
	u8 flags;
	fs_node_t* (*create)(struct _FS_FILE_SYSTEM*,_Bool,const char*,u8);
	_Bool (*delete)(struct _FS_FILE_SYSTEM*,fs_node_t*);
	fs_node_t* (*get_relative)(struct _FS_FILE_SYSTEM*,fs_node_t*,u8);
	_Bool (*set_relative)(struct _FS_FILE_SYSTEM*,fs_node_t*,u8,fs_node_t*);
	_Bool (*move_file)(struct _FS_FILE_SYSTEM*,fs_node_t*,fs_node_t*);
	u64 (*read)(struct _FS_FILE_SYSTEM*,fs_node_t*,u64,u8*,u64);
	u64 (*write)(struct _FS_FILE_SYSTEM*,fs_node_t*,u64,const u8*,u64);
	u64 (*get_size)(struct _FS_FILE_SYSTEM*,fs_node_t*);
	void (*flush_cache)(struct _FS_FILE_SYSTEM*);
} fs_file_system_config_t;



typedef struct _FS_FILE_SYSTEM{
	lock_t lock;
	const fs_file_system_config_t* config;
	fs_partition_config_t partition_config;
	u8 index;
	u8 flags;
	u8 name_length;
	char name[16];
	const drive_t* drive;
	fs_node_t* root;
	fs_node_allocator_t allocator;
	void* extra_data;
} fs_file_system_t;



void fs_init(void);



void* fs_create_file_system(const drive_t* drive,const fs_partition_config_t* partition_config,const fs_file_system_config_t* config,void* extra_data);



u8 fs_get_file_system_count(void);



const fs_file_system_t* fs_get_file_system(u8 fs_index);



void fs_set_boot_file_system(u8 fs_index);



void fs_set_half_installed_file_system(u8 fs_index);



void fs_set_previous_boot_file_system(u8 fs_index);



void fs_flush_cache(void);



void* fs_node_alloc(u8 fs_index,const char* name,u8 name_length);



_Bool fs_node_dealloc(fs_node_t* node);



fs_node_t* fs_node_get_by_id(fs_node_id_t id);



fs_node_t* fs_node_get_by_path(fs_node_t* root,const char* path,u8 type);



fs_node_t* fs_node_get_relative(fs_node_t* node,u8 relative);



_Bool fs_node_set_relative(fs_node_t* node,u8 relative,fs_node_t* other);



_Bool fs_node_move(fs_node_t* src_node,fs_node_t* dst_node);



_Bool fs_node_delete(fs_node_t* node);



fs_node_t* fs_node_get_child(fs_node_t* parent,const char* name,u8 name_length);



u64 fs_node_read(fs_node_t* node,u64 offset,void* buffer,u64 count);



u64 fs_node_write(fs_node_t* node,u64 offset,const void* buffer,u64 count);



u64 fs_node_get_size(fs_node_t* node);



#endif
