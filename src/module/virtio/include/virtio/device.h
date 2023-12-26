#ifndef _VIRTIO_DEVICE_H_
#define _VIRTIO_DEVICE_H_ 1
#include <kernel/handle/handle.h>
#include <kernel/lock/spinlock.h>
#include <kernel/tree/rb_tree.h>
#include <kernel/types.h>



typedef u16 virtio_device_type_t;



typedef struct _VIRTIO_DEVICE{
	handle_t handle;
	virtio_device_type_t type;
	u16 port;
	spinlock_t lock;
} virtio_device_t;



typedef struct _VIRTIO_DEVICE_DRIVER{
	const char* name;
	virtio_device_type_t type;
	u64 features;
} virtio_device_driver_t;



typedef struct _VIRTIO_DEVICE_DRIVER_NODE{
	rb_tree_node_t rb_node;
	const virtio_device_driver_t* driver;
} virtio_device_driver_node_t;



_Bool virtio_register_device_driver(const virtio_device_driver_t* driver);



_Bool virtio_unregister_device_driver(const virtio_device_driver_t* driver);



void virtio_locate_devices(void);



#endif
