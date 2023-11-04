#include <kernel/config/config.h>
#include <kernel/elf/elf.h>
#include <kernel/fs/fs.h>
#include <kernel/handle/handle.h>
#include <kernel/kernel.h>
#include <kernel/log/log.h>
#include <kernel/module/module.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#include <kernel/vfs/vfs.h>
#define KERNEL_LOG_NAME "os_loader"



#define EARLY_MODULE_ORDER_FILE "/boot/module/module_order.config"
#define LATE_MODULE_ORDER_FILE "/boot/module/module_order.config" // Same file name, different filesystem



static void _load_modules_from_order_file(const char* order_file_path){
	vfs_node_t* file=vfs_lookup(NULL,order_file_path);
	if (!file){
		panic("Unable to locate module order file");
	}
	config_t* config=config_load(file);
	for (config_item_t* item=config->head;item;item=item->next){
		module_load(item->key->data);
	}
	config_dealloc(config);
}



static _Bool _init(module_t* module){
	LOG("Loading early modules...");
	_load_modules_from_order_file(EARLY_MODULE_ORDER_FILE);
	LOG("Searching for boot filesystem...");
	filesystem_t* boot_fs=NULL;
	HANDLE_FOREACH(HANDLE_TYPE_FS){
		handle_acquire(handle);
		filesystem_t* fs=handle->object;
		for (u8 i=0;i<16;i++){
			if (fs->uuid[i]!=kernel_data.boot_fs_uuid[i]){
				goto _check_next_fs;
			}
		}
		boot_fs=fs;
		break;
_check_next_fs:
		handle_release(handle);
	}
	if (!boot_fs){
		panic("Unable to find boot filesystem");
	}
	vfs_mount(boot_fs,NULL);
	LOG("Loading late modules...");
	_load_modules_from_order_file(LATE_MODULE_ORDER_FILE);
#if KERNEL_COVERAGE_ENABLED
	module_load("coverage");
#endif
	LOG("Loading user shell...");
	if (!elf_load(vfs_lookup(NULL,"/shell.elf"))){
		panic("Unable to load user shell");
	}
	return 1;
}



static void _deinit(module_t* module){
	return;
}



MODULE_DECLARE(
	"os_loader",
	_init,
	_deinit
);
