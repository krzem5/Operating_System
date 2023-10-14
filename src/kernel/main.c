#include <kernel/acpi/acpi.h>
#include <kernel/aml/bus.h>
#include <kernel/aml/runtime.h>
#include <kernel/bios/bios.h>
#include <kernel/clock/clock.h>
#include <kernel/cpu/cpu.h>
#include <kernel/elf/elf.h>
#include <kernel/handle/handle.h>
#include <kernel/isr/isr.h>
#include <kernel/kernel.h>
#include <kernel/log/log.h>
#include <kernel/memory/kmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/network/layer2.h>
#include <kernel/partition/partition.h>
#include <kernel/pci/pci.h>
#include <kernel/random/random.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/serial/serial.h>
#include <kernel/types.h>
#define KERNEL_LOG_NAME "main"



static void _main_thread(void){
	ERROR("Main thread! (%p)");
}



void KERNEL_NORETURN KERNEL_NOCOVERAGE main(const kernel_data_t* bootloader_kernel_data){
	serial_init();
	cpu_check_features();
	LOG("Starting kernel...");
	kernel_init(bootloader_kernel_data);
	pmm_init();
	vmm_init();
	pmm_init_high_mem();
	kmm_init();
	clock_init();
	handle_init();
	isr_init();
	acpi_load();
	pci_enumerate();
	partition_load();
	kernel_load();
	aml_bus_enumerate();
	bios_get_system_data();
	network_layer2_init();
	random_init();
	serial_init_irq();
	scheduler_init();
	process_init();
	cpu_start_all_cores();
	scheduler_enqueue_thread(thread_new_kernel(process_kernel,(u64)_main_thread,0x200000,0));
	elf_load("/kernel/loader.elf");
	scheduler_enable();
	scheduler_start();
	for (;;);
}
