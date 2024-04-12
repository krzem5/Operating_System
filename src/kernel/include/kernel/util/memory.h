#ifndef _KERNEL_UTIL_MEMORY_H_
#define _KERNEL_UTIL_MEMORY_H_ 1
#include <kernel/types.h>



#define mem_copy(dst,src,length) ((__builtin_constant_p(length)?__mem_copy_inline:(mem_copy))((dst),(src),(length)))
#define mem_fill(ptr,length,value) ((__builtin_constant_p(length)?__mem_fill_inline:(mem_fill))((ptr),(length),(value)))



static KERNEL_INLINE void KERNEL_NOCOVERAGE __mem_copy_inline(void* dst,const void* src,u64 length){
	u8* dst_ptr=dst;
	const u8* src_ptr=src;
	for (u64 i=0;i<length;i++){
		dst_ptr[i]=src_ptr[i];
	}
}



static KERNEL_INLINE void KERNEL_NOCOVERAGE __mem_fill_inline(void* ptr,u64 length,u8 value){
	u8* ptr8=ptr;
	for (u64 i=0;i<length;i++){
		ptr8[i]=value;
	}
}



void (mem_copy)(void* dst,const void* src,u64 length);



void (mem_fill)(void* ptr,u64 length,u8 value);



#endif