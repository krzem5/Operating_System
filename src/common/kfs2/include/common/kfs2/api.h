#ifndef _COMMON_KFS2_API_H_
#define _COMMON_KFS2_API_H_ 1
#include <common/kfs2/bitmap.h>
#include <common/kfs2/structures.h>
#include <common/kfs2/util.h>



typedef void* (*kfs2_filesystem_page_alloc_callback_t)(u64);



typedef void (*kfs2_filesystem_page_dealloc_callback_t)(void*,u64);



typedef u64 (*kfs2_filesystem_block_read_callback_t)(void*,u64,void*,u64);



typedef u64 (*kfs2_filesystem_block_write_callback_t)(void*,u64,const void*,u64);



typedef struct _KFS2_FILESYSTEM_CONFIG{
	void* ctx;
	kfs2_filesystem_block_read_callback_t read_callback;
	kfs2_filesystem_block_write_callback_t write_callback;
	kfs2_filesystem_page_alloc_callback_t alloc_callback;
	kfs2_filesystem_page_dealloc_callback_t dealloc_callback;
	u64 block_size;
	u64 start_lba;
	u64 end_lba;
} kfs2_filesystem_config_t;



typedef struct _KFS2_FILESYSTEM{
	kfs2_filesystem_config_t config;
	kfs2_root_block_t root_block;
	u32 block_size_shift;
	kfs2_bitmap_t data_block_allocator;
	kfs2_bitmap_t inode_allocator;
} kfs2_filesystem_t;



bool kfs2_filesystem_format(const kfs2_filesystem_config_t* config,kfs2_filesystem_t* out);



bool kfs2_filesystem_init(const kfs2_filesystem_config_t* config,kfs2_filesystem_t* out);



void kfs2_filesystem_deinit(kfs2_filesystem_t* fs);



bool kfs2_filesystem_get_root(kfs2_filesystem_t* fs,kfs2_node_t* out);



void kfs2_filesystem_flush_root_block(kfs2_filesystem_t* fs);



bool kfs2_node_create(kfs2_filesystem_t* fs,kfs2_node_t* parent,const char* name,u32 name_length,u32 flags,kfs2_node_t* out);



bool kfs2_node_delete(kfs2_filesystem_t* fs,kfs2_node_t* node);



bool kfs2_node_lookup(kfs2_filesystem_t* fs,kfs2_node_t* parent,const char* name,u32 name_length,kfs2_node_t* out);



u64 kfs2_node_iterate(kfs2_filesystem_t* fs,kfs2_node_t* parent,u64 pointer,char* buffer,u32* buffer_length);



bool kfs2_node_link(kfs2_filesystem_t* fs,kfs2_node_t* parent,kfs2_node_t* child,const char* name,u32 name_length);



bool kfs2_node_unlink(kfs2_filesystem_t* fs,kfs2_node_t* parent,kfs2_node_t* child,const char* name,u32 name_length);



u64 kfs2_node_read(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 offset,void* buffer,u64 size);



u64 kfs2_node_write(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 offset,const void* buffer,u64 size);



u64 kfs2_node_resize(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size);



bool kfs2_node_flush(kfs2_filesystem_t* fs,kfs2_node_t* node);



#endif
