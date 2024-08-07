#ifndef _KERNEL_KERNEL_KERNEL_H_
#define _KERNEL_KERNEL_KERNEL_H_ 1
#include <common/kernel/kernel.h>
#include <kernel/types.h>



#define _KERNEL_DECLARE_SECTION(name) \
	static KERNEL_INLINE u64 KERNEL_NOCOVERAGE kernel_section_##name##_start(void){ \
		extern u64 __kernel_section_##name##_start[1]; \
		return (u64)(__kernel_section_##name##_start);  \
	} \
	static KERNEL_INLINE u64 KERNEL_NOCOVERAGE kernel_section_##name##_end(void){ \
		extern u64 __kernel_section_##name##_end[1]; \
		return (u64)(__kernel_section_##name##_end);  \
	}



typedef void (*kernel_initializer_t)(void);



extern kernel_data_t kernel_data;



static KERNEL_INLINE u64 KERNEL_NOCOVERAGE kernel_get_offset(void){
	return 0xffffffffc0000000ull;
}



_KERNEL_DECLARE_SECTION(kernel);
_KERNEL_DECLARE_SECTION(kernel_ue);
_KERNEL_DECLARE_SECTION(kernel_ur);
_KERNEL_DECLARE_SECTION(kernel_uw);
_KERNEL_DECLARE_SECTION(kernel_ex);
_KERNEL_DECLARE_SECTION(kernel_nx);
_KERNEL_DECLARE_SECTION(kernel_rw);
_KERNEL_DECLARE_SECTION(kernel_iw);

_KERNEL_DECLARE_SECTION(cpu_local);
_KERNEL_DECLARE_SECTION(early_early_initializers);
_KERNEL_DECLARE_SECTION(early_initializers);
_KERNEL_DECLARE_SECTION(initializers);
_KERNEL_DECLARE_SECTION(async_initializers);
_KERNEL_DECLARE_SECTION(gcov_info);



void kernel_init(const kernel_data_t* bootloader_kernel_data);



void kernel_adjust_memory_flags(void);



void kernel_early_execute_initializers(void);



void kernel_execute_initializers(void);



void kernel_adjust_memory_flags_after_init(void);



u64 kernel_gcov_info_data(u64* size);



u64 kernel_get_version(void);



const char* kernel_get_build_name(void);



const u8* kernel_get_boot_uuid(void);



#endif
