#include <common/kfs2/api.h>
#include <common/kfs2/bitmap.h>
#include <common/kfs2/chunk.h>
#include <common/kfs2/crc.h>
#include <common/kfs2/io.h>
#include <common/kfs2/structures.h>
#include <common/kfs2/util.h>



#define FNV_OFFSET_BASIS 0x811c9dc5
#define FNV_PRIME 0x01000193



static u8 _calculate_compressed_hash(const char* name,u32 name_length){
	u32 hash=FNV_OFFSET_BASIS;
	for (u32 i=0;i<name_length;i++){
		hash=(hash^name[i])*FNV_PRIME;
	}
	hash^=hash>>16;
	return hash^(hash>>8);
}



static void _node_resize_data_inline_inline(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	if (size<node->size){
		return;
	}
	mem_fill(node->data.inline_+node->size,size-node->size,0);
}



static void _node_resize_data_inline_single(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	kfs2_node_data_t old_data=node->data;
	for (u32 i=0;i<6;i++){
		node->data.single[i]=(i*KFS2_BLOCK_SIZE<size?kfs2_bitmap_alloc(fs,&(fs->data_block_allocator)):0);
	}
	void* buffer=fs->config.alloc_callback(1);
	mem_copy(buffer,old_data.inline_,node->size);
	kfs2_io_data_block_write(fs,node->data.single[0],buffer);
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_inline_double(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	kfs2_node_data_t old_data=node->data;
	node->data.double_=kfs2_bitmap_alloc(fs,&(fs->data_block_allocator));
	u64* buffer=fs->config.alloc_callback(1);
	for (u64 i=0;i<KFS2_BLOCK_SIZE/sizeof(u64);i++){
		buffer[i]=(i*KFS2_BLOCK_SIZE<size?kfs2_bitmap_alloc(fs,&(fs->data_block_allocator)):0);
	}
	kfs2_io_data_block_write(fs,node->data.double_,buffer);
	u64 first_block_address=buffer[0];
	mem_fill(buffer,KFS2_BLOCK_SIZE,0);
	mem_copy(buffer,old_data.inline_,node->size);
	kfs2_io_data_block_write(fs,first_block_address,buffer);
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_inline_triple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_inline_triple");
}



static void _node_resize_data_inline_quadruple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_inline_quadruple");
}



static void _node_resize_data_single_inline(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	u8 inline_buffer[48];
	if (!size){
		goto _skip_data_copy;
	}
	void* buffer=fs->config.alloc_callback(1);
	kfs2_io_data_block_read(fs,node->data.single[0],buffer);
	mem_copy(inline_buffer,buffer,size);
	fs->config.dealloc_callback(buffer,1);
_skip_data_copy:
	for (u64 i=0;i<6;i++){
		if (node->data.single[i]){
			kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),node->data.single[i]);
		}
	}
	mem_copy(node->data.inline_,inline_buffer,size);
}



static void _node_resize_data_single_single(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	for (u64 i=0;i<6;i++){
		if (i*KFS2_BLOCK_SIZE<size){
			if (!node->data.single[i]){
				node->data.single[i]=kfs2_bitmap_alloc(fs,&(fs->data_block_allocator));
			}
		}
		else if (node->data.single[i]){
			kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),node->data.single[i]);
			node->data.single[i]=0;
		}
	}
	if (size>node->size){
		return;
	}
	void* buffer=fs->config.alloc_callback(1);
	u64 block_address=node->data.single[size/KFS2_BLOCK_SIZE];
	u64 offset=size&(KFS2_BLOCK_SIZE-1);
	kfs2_io_data_block_read(fs,block_address,buffer);
	mem_fill(buffer+offset,KFS2_BLOCK_SIZE-offset,0);
	kfs2_io_data_block_write(fs,block_address,buffer);
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_single_double(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	kfs2_node_data_t old_data=node->data;
	node->data.double_=kfs2_bitmap_alloc(fs,&(fs->data_block_allocator));
	u64* buffer=fs->config.alloc_callback(1);
	mem_copy(buffer,old_data.single,6*sizeof(u64));
	for (u64 i=0;i<KFS2_BLOCK_SIZE/sizeof(u64)&&i*KFS2_BLOCK_SIZE<size;i++){
		if (!buffer[i]){
			buffer[i]=kfs2_bitmap_alloc(fs,&(fs->data_block_allocator));
		}
	}
	kfs2_io_data_block_write(fs,node->data.double_,buffer);
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_single_triple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_single_triple");
}



static void _node_resize_data_single_quadruple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_single_quadruple");
}



static void _node_resize_data_double_inline(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_double_inline");
}



static void _node_resize_data_double_single(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	u64* buffer=fs->config.alloc_callback(1);
	kfs2_io_data_block_read(fs,node->data.double_,buffer);
	kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),node->data.double_);
	for (u64 i=(size+KFS2_BLOCK_SIZE-1)/KFS2_BLOCK_SIZE;i<KFS2_BLOCK_SIZE/sizeof(u64);i++){
		if (buffer[i]){
			kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),buffer[i]);
			buffer[i]=0;
		}
	}
	mem_copy(node->data.single,buffer,6*sizeof(u64));
	u64 block_address=node->data.single[size/KFS2_BLOCK_SIZE];
	u64 offset=size&(KFS2_BLOCK_SIZE-1);
	kfs2_io_data_block_read(fs,block_address,buffer);
	mem_fill(((void*)buffer)+offset,KFS2_BLOCK_SIZE-offset,0);
	kfs2_io_data_block_write(fs,block_address,buffer);
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_double_double(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	u64* buffer=fs->config.alloc_callback(1);
	kfs2_io_data_block_read(fs,node->data.double_,buffer);
	for (u64 i=0;i<KFS2_BLOCK_SIZE/sizeof(u64);i++){
		if (i*KFS2_BLOCK_SIZE<size){
			if (!buffer[i]){
				buffer[i]=kfs2_bitmap_alloc(fs,&(fs->data_block_allocator));
			}
		}
		else if (buffer[i]){
			kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),buffer[i]);
			buffer[i]=0;
		}
	}
	kfs2_io_data_block_write(fs,node->data.double_,buffer);
	if (size<node->size){
		u64 block_address=buffer[size/KFS2_BLOCK_SIZE];
		u64 offset=size&(KFS2_BLOCK_SIZE-1);
		kfs2_io_data_block_read(fs,block_address,buffer);
		mem_fill(((void*)buffer)+offset,KFS2_BLOCK_SIZE-offset,0);
		kfs2_io_data_block_write(fs,block_address,buffer);
	}
	fs->config.dealloc_callback(buffer,1);
}



static void _node_resize_data_double_triple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_double_triple");
}



static void _node_resize_data_double_quadruple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_double_quadruple");
}



static void _node_resize_data_triple_inline(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_triple_inline");
}



static void _node_resize_data_triple_single(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_triple_single");
}



static void _node_resize_data_triple_double(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_triple_double");
}



static void _node_resize_data_triple_triple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_triple_triple");
}



static void _node_resize_data_triple_quadruple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_triple_quadruple");
}



static void _node_resize_data_quadruple_inline(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_quadruple_inline");
}



static void _node_resize_data_quadruple_single(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_quadruple_single");
}



static void _node_resize_data_quadruple_double(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_quadruple_double");
}



static void _node_resize_data_quadruple_triple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_quadruple_triple");
}



static void _node_resize_data_quadruple_quadruple(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	panic("_node_resize_data_quadruple_quadruple");
}



static void _node_resize(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	if (node->size==size){
		return;
	}
	u32 new_storage_type;
	if (size<=48){
		new_storage_type=KFS2_INODE_STORAGE_TYPE_INLINE;
	}
	else if (size<=6*KFS2_BLOCK_SIZE){
		new_storage_type=KFS2_INODE_STORAGE_TYPE_SINGLE;
	}
	else if (size<=((u64)KFS2_BLOCK_SIZE)*KFS2_BLOCK_SIZE/8){
		new_storage_type=KFS2_INODE_STORAGE_TYPE_DOUBLE;
	}
	else if (size<=((u64)KFS2_BLOCK_SIZE)*KFS2_BLOCK_SIZE*KFS2_BLOCK_SIZE/64){
		new_storage_type=KFS2_INODE_STORAGE_TYPE_TRIPLE;
	}
	else if (size<=((u64)KFS2_BLOCK_SIZE)*KFS2_BLOCK_SIZE*KFS2_BLOCK_SIZE*KFS2_BLOCK_SIZE/512){
		new_storage_type=KFS2_INODE_STORAGE_TYPE_QUADRUPLE;
	}
	else{
		panic("_node_resize: File too large");
	}
	u32 old_storage_type=node->flags&KFS2_INODE_STORAGE_MASK;
	if (old_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE){
		_node_resize_data_inline_inline(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE){
		_node_resize_data_inline_single(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		_node_resize_data_inline_double(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		_node_resize_data_inline_triple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		_node_resize_data_inline_quadruple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE){
		_node_resize_data_single_inline(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE){
		_node_resize_data_single_single(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		_node_resize_data_single_double(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		_node_resize_data_single_triple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		_node_resize_data_single_quadruple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE){
		_node_resize_data_double_inline(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE){
		_node_resize_data_double_single(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		_node_resize_data_double_double(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		_node_resize_data_double_triple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		_node_resize_data_double_quadruple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE){
		_node_resize_data_triple_inline(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE){
		_node_resize_data_triple_single(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		_node_resize_data_triple_double(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		_node_resize_data_triple_triple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		_node_resize_data_triple_quadruple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_INLINE){
		_node_resize_data_quadruple_inline(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_SINGLE){
		_node_resize_data_quadruple_single(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		_node_resize_data_quadruple_double(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		_node_resize_data_quadruple_triple(fs,node,size);
	}
	else if (old_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE&&new_storage_type==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		_node_resize_data_quadruple_quadruple(fs,node,size);
	}
	else{
		panic("_node_resize: invalid storage type");
	}
	node->size=size;
	node->flags=(node->flags&(~KFS2_INODE_STORAGE_MASK))|new_storage_type;
	kfs2_io_inode_write(fs,node);
}



static void _attach_child(kfs2_filesystem_t* fs,kfs2_node_t* parent,const char* name,u32 name_length,kfs2_node_t* child){
	child->rc++;
	kfs2_node_flush(fs,child);
	u16 new_entry_size=(sizeof(kfs2_directory_entry_t)+name_length+3)&0xfffc;
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	for (u64 offset=0;offset<parent->size;){
		if (offset-chunk.offset>=chunk.length){
			kfs2_chunk_read(fs,parent,offset,1,&chunk);
		}
		kfs2_directory_entry_t* entry=(kfs2_directory_entry_t*)(chunk.data+offset-chunk.offset);
		if (entry->name_length||entry->size<new_entry_size){
			offset+=entry->size;
			continue;
		}
		entry->inode=child->inode;
		entry->name_length=name_length;
		entry->name_compressed_hash=_calculate_compressed_hash(name,name_length);
		mem_copy(entry->name,name,name_length);
		if (entry->size-new_entry_size>=12){
			kfs2_directory_entry_t* next_entry=(kfs2_directory_entry_t*)(chunk.data+offset-chunk.offset+new_entry_size);
			next_entry->inode=0;
			next_entry->size=entry->size-new_entry_size;
			next_entry->name_length=0;
			entry->size=new_entry_size;
		}
		kfs2_chunk_write(fs,parent,&chunk);
		kfs2_chunk_deinit(fs,&chunk);
		return;
	}
	kfs2_chunk_deinit(fs,&chunk);
	_node_resize(fs,parent,(parent->size==48?KFS2_BLOCK_SIZE:parent->size+KFS2_BLOCK_SIZE));
	kfs2_chunk_read(fs,parent,parent->size-KFS2_BLOCK_SIZE,1,&chunk);
	if (parent->size>KFS2_BLOCK_SIZE){
		panic("_attach_child: update large parent");
		kfs2_chunk_deinit(fs,&chunk);
		return;
	}
	kfs2_directory_entry_t* entry;
	u64 offset=0;
	while (offset<48){
		entry=(kfs2_directory_entry_t*)(chunk.data+offset);
		offset+=entry->size;
	}
	if (!entry->name_length){
		offset-=entry->size;
		entry->size+=KFS2_BLOCK_SIZE-offset;
	}
	else{
		entry=(kfs2_directory_entry_t*)(chunk.data+offset);
		entry->size=KFS2_BLOCK_SIZE-offset;
	}
	entry->inode=child->inode;
	entry->name_length=name_length;
	entry->name_compressed_hash=_calculate_compressed_hash(name,name_length);
	mem_copy(entry->name,name,name_length);
	kfs2_directory_entry_t* next_entry=(kfs2_directory_entry_t*)(chunk.data+offset+new_entry_size);
	next_entry->inode=0;
	next_entry->size=entry->size-new_entry_size;
	next_entry->name_length=0;
	entry->size=new_entry_size;
	kfs2_chunk_write(fs,parent,&chunk);
	kfs2_chunk_deinit(fs,&chunk);
}



bool kfs2_filesystem_init(const kfs2_filesystem_config_t* config,kfs2_filesystem_t* out){
	if (config->block_size>4096){
		return 0;
	}
	out->config=*config;
	out->block_size_shift=63-__builtin_clzll(KFS2_BLOCK_SIZE/out->config.block_size);
	void* buffer=out->config.alloc_callback(1);
	kfs2_root_block_t* root_block=(kfs2_root_block_t*)buffer;
	if (out->config.read_callback(out->config.ctx,out->config.start_lba,buffer,1)!=1||root_block->signature!=KFS2_ROOT_BLOCK_SIGNATURE||!kfs2_verify_crc(root_block,sizeof(kfs2_root_block_t))){
		out->config.dealloc_callback(buffer,1);
		return 0;
	}
	out->root_block=*root_block;
	out->config.dealloc_callback(buffer,1);
	kfs2_bitmap_init(out,&(out->data_block_allocator),out->root_block.data_block_allocation_bitmap_offsets,out->root_block.data_block_allocation_bitmap_highest_level_length);
	kfs2_bitmap_init(out,&(out->inode_allocator),out->root_block.inode_allocation_bitmap_offsets,out->root_block.inode_allocation_bitmap_highest_level_length);
	return 1;
}



void kfs2_filesystem_deinit(kfs2_filesystem_t* fs){
	kfs2_bitmap_deinit(fs,&(fs->data_block_allocator));
	kfs2_bitmap_deinit(fs,&(fs->inode_allocator));
}



bool kfs2_filesystem_get_root(kfs2_filesystem_t* fs,kfs2_node_t* out){
	kfs2_io_inode_read(fs,0,out);
	return 1;
}



void kfs2_filesystem_flush_root_block(kfs2_filesystem_t* fs){
	kfs2_insert_crc(&(fs->root_block),sizeof(kfs2_root_block_t));
	kfs2_root_block_t* buffer=fs->config.alloc_callback(1);
	*buffer=fs->root_block;
	fs->config.write_callback(fs->config.ctx,fs->config.start_lba,buffer,1);
	fs->config.dealloc_callback(buffer,1);
}



bool kfs2_node_create(kfs2_filesystem_t* fs,kfs2_node_t* parent,const char* name,u32 name_length,u32 flags,kfs2_node_t* out){
	if ((parent->flags&KFS2_INODE_TYPE_MASK)!=KFS2_INODE_TYPE_DIRECTORY||name_length>255){
		return 0;
	}
	u32 inode=kfs2_bitmap_alloc(fs,&(fs->inode_allocator));
	if (!inode){
		return 0;
	}
	if ((flags&KFS2_INODE_TYPE_MASK)==KFS2_INODE_TYPE_DIRECTORY){
		out->size=48;
		out->flags=KFS2_INODE_TYPE_DIRECTORY|KFS2_INODE_STORAGE_TYPE_INLINE;
		kfs2_directory_entry_t* entry=(kfs2_directory_entry_t*)(out->data.inline_);
		entry->inode=0;
		entry->size=48;
		entry->name_length=0;
	}
	else{
		out->size=0;
		out->flags=(flags&KFS2_INODE_TYPE_MASK)|KFS2_INODE_STORAGE_TYPE_INLINE;
	}
	out->time_access=0;
	out->time_modify=0;
	out->time_change=0;
	out->time_birth=0;
	out->gid=0;
	out->uid=0;
	out->rc=0;
	out->inode=inode;
	_attach_child(fs,parent,name,name_length,out);
	return 1;
}



bool kfs2_node_delete(kfs2_filesystem_t* fs,kfs2_node_t* node){
	if (node->inode==0xffffffff){
		return 0;
	}
	if (node->rc){
		panic("kfs2_node_delete: node still referenced");
	}
	if ((node->flags&KFS2_INODE_STORAGE_MASK)==KFS2_INODE_STORAGE_TYPE_SINGLE){
		for (u32 i=0;i<6;i++){
			if (node->data.single[i]){
				kfs2_bitmap_dealloc(fs,&(fs->data_block_allocator),node->data.single[i]);
			}
		}
	}
	else if ((node->flags&KFS2_INODE_STORAGE_MASK)==KFS2_INODE_STORAGE_TYPE_DOUBLE){
		panic("kfs2_node_delete: KFS2_INODE_STORAGE_TYPE_DOUBLE");
	}
	else if ((node->flags&KFS2_INODE_STORAGE_MASK)==KFS2_INODE_STORAGE_TYPE_TRIPLE){
		panic("kfs2_node_delete: KFS2_INODE_STORAGE_TYPE_TRIPLE");
	}
	else if ((node->flags&KFS2_INODE_STORAGE_MASK)==KFS2_INODE_STORAGE_TYPE_QUADRUPLE){
		panic("kfs2_node_delete: KFS2_INODE_STORAGE_TYPE_QUADRUPLE");
	}
	kfs2_bitmap_dealloc(fs,&(fs->inode_allocator),node->inode);
	u32 inode=node->inode;
	mem_fill(node,0,sizeof(kfs2_node_t));
	node->inode=inode;
	kfs2_io_inode_write(fs,node);
	node->inode=0xffffffff;
	return 1;
}



bool kfs2_node_lookup(kfs2_filesystem_t* fs,kfs2_node_t* parent,const char* name,u32 name_length,kfs2_node_t* out){
	if (parent->inode==0xffffffff||!parent->size||(parent->flags&KFS2_INODE_TYPE_MASK)!=KFS2_INODE_TYPE_DIRECTORY||!name_length){
		return 0;
	}
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	u64 offset=0;
	u8 compressed_hash=_calculate_compressed_hash(name,name_length);
	while (offset<parent->size){
		if (offset-chunk.offset>=chunk.length){
			kfs2_chunk_read(fs,parent,offset,1,&chunk);
		}
		kfs2_directory_entry_t* entry=(kfs2_directory_entry_t*)(chunk.data+offset-chunk.offset);
		if (entry->name_length!=name_length||entry->name_compressed_hash!=compressed_hash){
			goto _skip_entry;
		}
		for (u16 i=0;i<name_length;i++){
			if (entry->name[i]!=name[i]){
				goto _skip_entry;
			}
		}
		u32 inode=entry->inode;
		kfs2_chunk_deinit(fs,&chunk);
		kfs2_io_inode_read(fs,inode,out);
		return 1;
	_skip_entry:
		offset+=entry->size;
	}
	kfs2_chunk_deinit(fs,&chunk);
	return 0;
}



u64 kfs2_node_iterate(kfs2_filesystem_t* fs,kfs2_node_t* parent,u64 pointer,char* buffer,u32* buffer_length){
	if (parent->inode==0xffffffff||pointer>=parent->size||(parent->flags&KFS2_INODE_TYPE_MASK)!=KFS2_INODE_TYPE_DIRECTORY){
		return 0;
	}
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	while (pointer<parent->size){
		if (pointer-chunk.offset>=chunk.length){
			kfs2_chunk_read(fs,parent,pointer,1,&chunk);
		}
		kfs2_directory_entry_t* entry=(kfs2_directory_entry_t*)(chunk.data+pointer-chunk.offset);
		pointer+=entry->size;
		if (!entry->name_length){
			continue;
		}
		if (entry->name_length<*buffer_length){
			*buffer_length=entry->name_length;
		}
		mem_copy(buffer,entry->name,*buffer_length);
		kfs2_chunk_deinit(fs,&chunk);
		return pointer;
	}
	kfs2_chunk_deinit(fs,&chunk);
	return 0;
}



bool kfs2_node_link(kfs2_filesystem_t* fs,kfs2_node_t* parent,kfs2_node_t* child,const char* name,u32 name_length){
	panic("kfs2_node_link");
	return 0;
}



bool kfs2_node_unlink(kfs2_filesystem_t* fs,kfs2_node_t* parent,kfs2_node_t* child,const char* name,u32 name_length){
	if (parent->inode==0xffffffff||!parent->size||(parent->flags&KFS2_INODE_TYPE_MASK)!=KFS2_INODE_TYPE_DIRECTORY||!name_length||child->inode==0xffffffff){
		return 0;
	}
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	u64 offset=0;
	u8 compressed_hash=_calculate_compressed_hash(name,name_length);
	while (offset<parent->size){
		if (offset-chunk.offset>=chunk.length){
			kfs2_chunk_read(fs,parent,offset,1,&chunk);
		}
		kfs2_directory_entry_t* entry=(kfs2_directory_entry_t*)(chunk.data+offset-chunk.offset);
		if (entry->inode!=child->inode||entry->name_length!=name_length||entry->name_compressed_hash!=compressed_hash){
			goto _skip_entry;
		}
		for (u16 i=0;i<name_length;i++){
			if (entry->name[i]!=name[i]){
				goto _skip_entry;
			}
		}
		entry->inode=0;
		entry->name_length=0;
		mem_fill(entry->name,0,entry->name_length);
		kfs2_chunk_write(fs,parent,&chunk);
		kfs2_chunk_deinit(fs,&chunk);
		child->rc--;
		kfs2_io_inode_write(fs,child);
		return 1;
	_skip_entry:
		offset+=entry->size;
	}
	kfs2_chunk_deinit(fs,&chunk);
	return 0;
}



u64 kfs2_node_read(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 offset,void* buffer,u64 size){
	if (node->inode==0xffffffff||offset>=node->size){
		return 0;
	}
	if (offset+size>=node->size){
		size=node->size-offset;
	}
	if (!size){
		return 0;
	}
	u64 out=size;
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	size+=offset;
	while (offset<size){
		if (offset-chunk.offset>=chunk.length){
			kfs2_chunk_read(fs,node,offset,1,&chunk);
		}
		u64 padding=offset-chunk.offset;
		u64 read_size=(chunk.length-padding>size-offset?size-offset:chunk.length-padding);
		mem_copy(buffer,chunk.data+padding,read_size);
		buffer+=read_size;
		offset+=chunk.length-padding;
	}
	kfs2_chunk_deinit(fs,&chunk);
	return out;
}



u64 kfs2_node_write(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 offset,const void* buffer,u64 size){
	if (node->inode==0xffffffff||offset>=node->size){
		return 0;
	}
	if (offset+size>=node->size){
		size=node->size-offset;
	}
	if (!size){
		return 0;
	}
	u64 out=size;
	kfs2_data_chunk_t chunk;
	kfs2_chunk_init(&chunk);
	size+=offset;
	while (offset<size){
		kfs2_chunk_read(fs,node,offset,(offset&(KFS2_BLOCK_SIZE-1))||(size-offset<KFS2_BLOCK_SIZE),&chunk);
		u64 padding=offset-chunk.offset;
		u64 write_size=(chunk.length-padding>size-offset?size-offset:chunk.length-padding);
		mem_copy(chunk.data+padding,buffer,write_size);
		buffer+=write_size;
		offset+=write_size;
		kfs2_chunk_write(fs,node,&chunk);
	}
	kfs2_chunk_deinit(fs,&chunk);
	return out;
}



u64 kfs2_node_resize(kfs2_filesystem_t* fs,kfs2_node_t* node,u64 size){
	_node_resize(fs,node,size);
	return node->size;
}



bool kfs2_node_flush(kfs2_filesystem_t* fs,kfs2_node_t* node){
	kfs2_io_inode_write(fs,node);
	return 1;
}
