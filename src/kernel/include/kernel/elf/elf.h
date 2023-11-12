#ifndef _KERNEL_ELF_ELF_H_
#define _KERNEL_ELF_ELF_H_ 1



#define ELF_AUXV_PLATFORM "x86_64"
#define ELF_AUXV_RANDOM_DATA_SIZE 16



_Bool elf_load(const char* path);



#endif
