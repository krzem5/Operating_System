#include <kernel/cpu/cpu.h>
#include <kernel/isr/isr.h>
#include <kernel/log/log.h>
#include <kernel/memory/mmap.h>
#include <kernel/memory/vmm.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/types.h>
#define KERNEL_LOG_NAME "pf"



_Bool pf_handle_fault(isr_state_t* isr_state){
	u64 address=vmm_get_fault_address()&(-PAGE_SIZE);
	if (!address||(isr_state->error&1)||!CPU_HEADER_DATA->current_thread){
		return 0;
	}
	mmap_region_t* region=mmap_lookup(&(THREAD_DATA->process->mmap),address);
	if (!region){
		return 0;
	}
	vmm_map_page(&(THREAD_DATA->process->pagemap),pmm_alloc_zero(1,region->pmm_counter,0),address,mmap_get_vmm_flags(region));
	vmm_invalidate_tlb_entry(address);
	return 1;
}
