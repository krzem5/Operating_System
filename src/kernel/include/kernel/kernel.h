#ifndef _KERNEL_KERNEL_H_
#define _KERNEL_KERNEL_H_ 1
#include <kernel/types.h>



#define _KERNEL_DECLARE_SECTION(name) \
	static KERNEL_INLINE u64 KERNEL_CORE_CODE KERNEL_NOCOVERAGE kernel_section_##name##_start(void){ \
		extern u64 __KERNEL_SECTION_##name##_START__[1]; \
		return (u64)(__KERNEL_SECTION_##name##_START__);  \
	} \
	static KERNEL_INLINE u64 KERNEL_CORE_CODE KERNEL_NOCOVERAGE kernel_section_##name##_end(void){ \
		extern u64 __KERNEL_SECTION_##name##_END__[1]; \
		return (u64)(__KERNEL_SECTION_##name##_END__);  \
	}



typedef struct __attribute__((packed)) _KERNEL_DATA{
	u16 mmap_size;
	u8 _padding[6];
	struct{
		u64 base;
		u64 length;
		u32 type;
		u8 _padding[4];
	} mmap[42];
	u64 first_free_address;
	u64 rsdp_address;
	u64 smbios_address;
} kernel_data_t;



extern const u64 __core_version;
extern const u64 kernel_symbols[];
extern kernel_data_t kernel_data;



static KERNEL_INLINE u64 KERNEL_CORE_CODE KERNEL_NOCOVERAGE kernel_get_version(void){
	return __core_version;
}



static KERNEL_INLINE u64 KERNEL_CORE_CODE KERNEL_NOCOVERAGE kernel_get_offset(void){
	return 0xffffffffc0000000ull;
}



_KERNEL_DECLARE_SECTION(address_range);
_KERNEL_DECLARE_SECTION(core);
_KERNEL_DECLARE_SECTION(core_ex);
_KERNEL_DECLARE_SECTION(core_nx);
_KERNEL_DECLARE_SECTION(core_rw);
_KERNEL_DECLARE_SECTION(kernel);
_KERNEL_DECLARE_SECTION(kernel_ex);
_KERNEL_DECLARE_SECTION(kernel_nx);
_KERNEL_DECLARE_SECTION(kernel_rw);
_KERNEL_DECLARE_SECTION(common);
_KERNEL_DECLARE_SECTION(common_ex);
_KERNEL_DECLARE_SECTION(common_rw);
_KERNEL_DECLARE_SECTION(bss);

_KERNEL_DECLARE_SECTION(pmm_counter);
_KERNEL_DECLARE_SECTION(cpu_local);
_KERNEL_DECLARE_SECTION(handle);
_KERNEL_DECLARE_SECTION(sandbox);



void kernel_init(const kernel_data_t* bootloader_kernel_data);



void kernel_load(void);



void kernel_adjust_memory_flags(void);



const char* kernel_lookup_symbol(u64 address,u64* offset);



#endif
