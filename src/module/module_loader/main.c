#include <kernel/config/config.h>
#include <kernel/elf/elf.h>
#include <kernel/error/error.h>
#include <kernel/initramfs/initramfs.h>
#include <kernel/log/log.h>
#include <kernel/module/module.h>
#include <kernel/types.h>
#include <kernel/util/string.h>
#include <kernel/util/util.h>
#include <kernel/vfs/vfs.h>
#define KERNEL_LOG_NAME "module_loader"



#define MODULE_ORDER_FILE "/boot/module/module_order.config"



static void _load_modules_from_order_file(bool early){
	vfs_node_t* file=vfs_lookup(NULL,MODULE_ORDER_FILE,0,0,0);
	if (!file){
		panic("Unable to locate module order file");
	}
	config_tag_t* root_tag=config_load_from_file(file,NULL);
	vfs_node_unref(file);
	for (config_tag_t* tag=config_tag_iter_start(root_tag);tag;tag=config_tag_iter_next(root_tag,tag)){
		if (tag->type!=CONFIG_TAG_TYPE_NONE&&tag->type!=CONFIG_TAG_TYPE_STRING){
			continue;
		}
		if (early!=(tag->type==CONFIG_TAG_TYPE_STRING&&str_equal(tag->string->data,"early"))){
			continue;
		}
#ifdef KERNEL_COVERAGE
		if (tag->type==CONFIG_TAG_TYPE_STRING&&str_equal(tag->string->data,"not-test")){
			continue;
		}
#else
		if (tag->type==CONFIG_TAG_TYPE_STRING&&str_equal(tag->string->data,"test")){
			continue;
		}
#endif
		module_load(tag->name->data);
	}
	config_tag_delete(root_tag);
}



MODULE_PREINIT(){
	LOG("Loading early modules...");
	_load_modules_from_order_file(1);
	LOG("Unloading initramfs...");
	initramfs_unload();
	LOG("Loading modules...");
	_load_modules_from_order_file(0);
	LOG("Loading user shell...");
#ifndef KERNEL_COVERAGE
	const char*const argv[]={
		"/bin/serial_terminal",
		"-i","/dev/ser/in",
		"-o","/dev/ser/out",
		"-s",
		"-",
		"/bin/shell"
	};
	if (IS_ERROR(elf_load("/bin/serial_terminal",sizeof(argv)/sizeof(const char*),argv,0,NULL,0))){
		panic("Unable to load user shell");
	}
#ifdef KERNEL_RELEASE
	LOG("Masking log types...");
	log_mask_type(LOG_TYPE_INFO);
	log_mask_type(LOG_TYPE_LOG);
#endif
#endif
	return 0;
}



MODULE_DECLARE(MODULE_FLAG_PREVENT_LOADS);
