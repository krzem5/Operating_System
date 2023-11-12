#include <devfs/fs.h>
#include <dynamicfs/dynamicfs.h>
#include <kernel/format/format.h>
#include <kernel/log/log.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/pipe/pipe.h>
#include <kernel/scheduler/load_balancer.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/serial/serial.h>
#include <kernel/vfs/node.h>
#define KERNEL_LOG_NAME "devfs_serial"



#define SERIAL_BUFFER_SIZE 256



static void _stdin_callback(vfs_node_t* node,serial_port_t* port){
	u8 buffer[1];
	while (1){
		vfs_node_write(node,0,buffer,serial_recv(port,buffer,1),0);
	}
}



static void _stdout_callback(vfs_node_t* node,serial_port_t* port){
	u8 buffer[SERIAL_BUFFER_SIZE];
	while (1){
		serial_send(port,buffer,vfs_node_read(node,0,buffer,SERIAL_BUFFER_SIZE,0));
	}
}



static void _create_pipe(vfs_node_t* parent,const char* name,void* callback,serial_port_t* port){
	SMM_TEMPORARY_STRING name_string=smm_alloc(name,0);
	thread_t* thread=thread_new_kernel_thread(process_kernel,callback,0x10000,2,pipe_create(parent,name_string),port);
	thread->priority=SCHEDULER_PRIORITY_HIGH;
	scheduler_enqueue_thread(thread);
}



void devfs_serial_init(void){
	LOG("Creating serial subsystem...");
	vfs_node_t* root=dynamicfs_create_node(devfs->root,"serial",VFS_NODE_TYPE_DIRECTORY,NULL,NULL,NULL);
	for (u8 i=0;i<SERIAL_PORT_COUNT;i++){
		serial_port_t* port=serial_ports+i;
		if (!port->io_port){
			continue;
		}
		char buffer[8];
		format_string(buffer,8,"ser%u",i);
		vfs_node_t* node=dynamicfs_create_node(root,buffer,VFS_NODE_TYPE_DIRECTORY,NULL,NULL,NULL);
		dynamicfs_create_data_node(node,"id","%u",i);
		_create_pipe(node,"in",_stdin_callback,port);
		_create_pipe(node,"out",_stdout_callback,port);
		dynamicfs_create_link_node(devfs->root,buffer,"serial/%s",buffer);
	}
	if (serial_default_port){
		dynamicfs_create_link_node(devfs->root,"ser","ser%u",serial_default_port-serial_ports);
	}
}