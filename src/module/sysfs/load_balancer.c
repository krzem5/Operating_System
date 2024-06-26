#include <dynamicfs/dynamicfs.h>
#include <kernel/cpu/cpu.h>
#include <kernel/format/format.h>
#include <kernel/log/log.h>
#include <kernel/module/module.h>
#include <kernel/scheduler/load_balancer.h>
#include <kernel/vfs/node.h>
#include <sysfs/fs.h>
#define KERNEL_LOG_NAME "sysfs_load_balancer"



MODULE_POSTINIT(){
	LOG("Creating load balancer subsystem...");
	vfs_node_t* root=dynamicfs_create_node(sysfs->root,"lb",VFS_NODE_TYPE_DIRECTORY,NULL,NULL,NULL);
	dynamicfs_set_root_only(root);
	for (u16 i=0;i<cpu_count;i++){
		char buffer[16];
		format_string(buffer,16,"%u",i);
		vfs_node_t* node=dynamicfs_create_node(root,buffer,VFS_NODE_TYPE_DIRECTORY,NULL,NULL,NULL);
		dynamicfs_set_root_only(node);
		const scheduler_load_balancer_stats_t* stats=scheduler_load_balancer_get_stats(i);
		vfs_node_unref(dynamicfs_set_root_only(dynamicfs_create_node(node,"free_slots",VFS_NODE_TYPE_FILE,NULL,dynamicfs_integer_read_callback,(void*)(&(stats->free_slot_count)))));
		vfs_node_unref(dynamicfs_set_root_only(dynamicfs_create_node(node,"used_slots",VFS_NODE_TYPE_FILE,NULL,dynamicfs_integer_read_callback,(void*)(&(stats->used_slot_count)))));
		vfs_node_unref(node);
	}
	vfs_node_unref(root);
}
