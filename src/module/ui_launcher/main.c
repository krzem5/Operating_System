#include <kernel/elf/elf.h>
#include <kernel/error/error.h>
#include <kernel/log/log.h>
#include <kernel/module/module.h>
#include <kernel/util/util.h>
#include <kernel/vfs/vfs.h>
#include <ui/common.h>
#define KERNEL_LOG_NAME "ui_launcher"



#define UI_EXECUTABLE_FILE_PATH "/bin/ui"



MODULE_PREINIT(){
	if (!vfs_lookup(NULL,"/bin/ui",0,0,0)){
		WARN("UI executable not found");
		return 0;
	}
	error_t process=elf_load(UI_EXECUTABLE_FILE_PATH,0,NULL,0,NULL,0);
	if (IS_ERROR(process)){
		panic("Unable to load UI");
	}
	ui_common_set_process(process);
	return 0;
}



MODULE_DECLARE(MODULE_FLAG_PREVENT_LOADS);
