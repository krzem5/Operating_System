#ifndef _LINKER_SEARCH_PATH_H_
#define _LINKER_SEARCH_PATH_H_ 1
#include <sys/types.h>



u64 linker_search_path_find_library(const char* name,char* buffer,u32 buffer_length);



const char* linker_search_path_update_search_directories(const char* new);



#endif
