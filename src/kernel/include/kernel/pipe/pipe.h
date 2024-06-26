#ifndef _KERNEL_PIPE_PIPE_H_
#define _KERNEL_PIPE_PIPE_H_ 1
#include <kernel/error/error.h>
#include <kernel/memory/smm.h>
#include <kernel/types.h>
#include <kernel/vfs/node.h>



#define PIPE_BUFFER_SIZE 0x100000



vfs_node_t* pipe_create(vfs_node_t* parent,const string_t* name);



error_t pipe_close(vfs_node_t* node);



#endif
