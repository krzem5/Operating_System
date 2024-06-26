#include <common/kfs2/api.h>
#include <common/kfs2/bitmap.h>
#include <common/kfs2/crc.h>
#include <common/kfs2/structures.h>
#include <common/kfs2/util.h>



static void _store_data(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator,u32 level){
	if (fs->config.write_callback(fs->config.ctx,fs->config.start_lba+((allocator->cache+level)->block_index<<fs->block_size_shift),(allocator->cache+level)->data,1<<fs->block_size_shift)!=(1<<fs->block_size_shift)){
		panic("_store_data: I/O error");
	}
}



static u64* _fetch_data(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator,u32 level,u64 offset){
	u64 block_index=(allocator->cache+level)->offset+offset*sizeof(u64)/KFS2_BLOCK_SIZE;
	if ((allocator->cache+level)->block_index!=block_index){
		if (fs->config.read_callback(fs->config.ctx,fs->config.start_lba+(block_index<<fs->block_size_shift),(allocator->cache+level)->data,1<<fs->block_size_shift)!=(1<<fs->block_size_shift)){
			panic("_fetch_data: I/O error");
		}
		(allocator->cache+level)->block_index=block_index;
	}
	return (allocator->cache+level)->data+(offset&(KFS2_BLOCK_SIZE/sizeof(u64)-1));
}



static void _find_next_highest_level_offset(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator){
	for (;allocator->highest_level_offset<allocator->highest_level_length&&!(*_fetch_data(fs,allocator,KFS2_BITMAP_LEVEL_COUNT-1,allocator->highest_level_offset));allocator->highest_level_offset++);
}



void kfs2_bitmap_init(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator,const u64* bitmap_offsets,u32 highest_level_length){
	allocator->highest_level_length=highest_level_length;
	allocator->highest_level_offset=0;
	for (u32 i=0;i<KFS2_BITMAP_LEVEL_COUNT;i++){
		(allocator->cache+i)->offset=bitmap_offsets[i];
		(allocator->cache+i)->block_index=0xffffffffffffffffull;
		(allocator->cache+i)->data=fs->config.alloc_callback(1);
	}
	_find_next_highest_level_offset(fs,allocator);
}



void kfs2_bitmap_deinit(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator){
	for (u32 i=0;i<KFS2_BITMAP_LEVEL_COUNT;i++){
		fs->config.dealloc_callback((allocator->cache+i)->data,1);
	}
}



u64 kfs2_bitmap_alloc(kfs2_filesystem_t* fs,kfs2_bitmap_t* allocator){
	if (allocator->highest_level_offset==allocator->highest_level_length){
		return 0;
	}
	u64 out=allocator->highest_level_offset;
	u64* ptr[KFS2_BITMAP_LEVEL_COUNT];
	for (u32 i=KFS2_BITMAP_LEVEL_COUNT;i;){
		i--;
		ptr[i]=_fetch_data(fs,allocator,i,out);
		out=(out<<6)|(__builtin_ffsll(*(ptr[i]))-1);
	}
	u32 i=0;
	for (;i<KFS2_BITMAP_LEVEL_COUNT;i++){
		*(ptr[i])&=(*(ptr[i]))-1;
		_store_data(fs,allocator,i);
		if (*(ptr[i])){
			break;
		}
	}
	if (i==KFS2_BITMAP_LEVEL_COUNT){
		allocator->highest_level_offset++;
		_find_next_highest_level_offset(fs,allocator);
	}
	return out;
}



void kfs2_bitmap_dealloc(struct _KFS2_FILESYSTEM* fs,kfs2_bitmap_t* allocator,u64 index){
	for (u32 i=0;i<KFS2_BITMAP_LEVEL_COUNT;i++){
		u64 mask=1ull<<(index&63);
		index>>=6;
		u64* ptr=_fetch_data(fs,allocator,i,index);
		if ((*ptr)&mask){
			break;
		}
		(*ptr)|=mask;
		_store_data(fs,allocator,i);
	}
	if (index<allocator->highest_level_offset){
		allocator->highest_level_offset=index;
	}
}
