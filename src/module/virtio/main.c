#include <kernel/module/module.h>
#include <virtio/device.h>



static _Bool _init(module_t* module){
	virtio_locate_devices();
	return 1;
}



static void _deinit(module_t* module){
	return;
}



MODULE_DECLARE(
	_init,
	_deinit,
	0
);
