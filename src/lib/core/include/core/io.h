#ifndef _CORE_IO_H_
#define _CORE_IO_H_ 1
#include <core/types.h>



void printf(const char* template,...);



void print_buffer(const void* buffer,u32 length);



void putchar(char c);



char getchar(void);



int getchar_timeout(u64 timeout);



#endif