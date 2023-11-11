#include <kernel/elf/structures.h>
#include <kernel/format/format.h>
#include <kernel/kernel.h>
#include <kernel/lock/spinlock.h>
#include <kernel/log/log.h>
#include <kernel/memory/mmap.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/module/module.h>
#include <kernel/mp/process.h>
#include <kernel/symbol/symbol.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#include <kernel/vfs/node.h>
#include <kernel/vfs/vfs.h>
#define KERNEL_LOG_NAME "module"



#define MODULE_ROOT_DIRECTORY "/boot/module"



static pmm_counter_descriptor_t _module_image_pmm_counter=PMM_COUNTER_INIT_STRUCT("module_image");
static pmm_counter_descriptor_t _module_omm_pmm_counter=PMM_COUNTER_INIT_STRUCT("omm_module");
static omm_allocator_t _module_allocator=OMM_ALLOCATOR_INIT_STRUCT("module",sizeof(module_t),8,4,&_module_omm_pmm_counter);

static spinlock_t _module_global_lock=SPINLOCK_INIT_STRUCT;



HANDLE_DECLARE_TYPE(MODULE,{
	module_t* module=handle->object;
	ERROR("Delete MODULE: %s",module->descriptor->name);
	omm_dealloc(&_module_allocator,module);
});



static void _module_alloc_region(module_address_range_t* region){
	region->size=pmm_align_up_address((region->size?region->size:1));
	mmap_region_t* mmap_region=mmap_alloc(&process_kernel_image_mmap,0,region->size,&_module_image_pmm_counter,MMAP_REGION_FLAG_COMMIT|MMAP_REGION_FLAG_VMM_READWRITE,NULL);
	if (!mmap_region){
		panic("Unable to reserve module section memory");
	}
	region->base=mmap_region->rb_node.key;
}



static void _map_section_addresses(void* file_data,const elf_hdr_t* header,module_t* module){
	INFO("Mapping section addresses...");
	elf_shdr_t* section_header=file_data+header->e_shoff+header->e_shstrndx*sizeof(elf_shdr_t);
	const char* string_table=file_data+section_header->sh_offset;
	module->ex_region.size=0;
	module->nx_region.size=0;
	module->rw_region.size=0;
	section_header=file_data+header->e_shoff;
	for (u16 i=0;i<header->e_shnum;i++){
		if (!(section_header->sh_flags&SHF_ALLOC)){
			section_header++;
			continue;
		}
		u64* var=NULL;
		switch (section_header->sh_flags&(SHF_WRITE|SHF_EXECINSTR)){
			case 0:
				var=&(module->nx_region.size);
				break;
			case SHF_WRITE:
				var=&(module->rw_region.size);
				break;
			case SHF_EXECINSTR:
				var=&(module->ex_region.size);
				break;
			default:
				panic("Invalid section flag combination");
		}
		if (section_header->sh_addralign){
			*var+=(-*var)&(section_header->sh_addralign-1);
		}
		*var+=section_header->sh_size;
		section_header++;
	}
	_module_alloc_region(&(module->ex_region));
	_module_alloc_region(&(module->nx_region));
	_module_alloc_region(&(module->rw_region));
	INFO("Regions: EX: %v, NX: %v, RW: %v",module->ex_region.size,module->nx_region.size,module->rw_region.size);
	module->gcov_info.base=0;
	module->gcov_info.size=0;
	section_header=file_data+header->e_shoff;
	u64 ex_base=module->ex_region.base;
	u64 nx_base=module->nx_region.base;
	u64 rw_base=module->rw_region.base;
	module->descriptor=NULL;
	INFO("Allocating sections...");
	for (u16 i=0;i<header->e_shnum;i++){
		if (!(section_header->sh_flags&SHF_ALLOC)){
			section_header->sh_addr=0;
			section_header++;
			continue;
		}
		u64* var=NULL;
		switch (section_header->sh_flags&(SHF_WRITE|SHF_EXECINSTR)){
			case 0:
				var=&(nx_base);
				break;
			case SHF_WRITE:
				var=&(rw_base);
				break;
			case SHF_EXECINSTR:
				var=&(ex_base);
				break;
		}
		if (section_header->sh_addralign){
			*var+=(-*var)&(section_header->sh_addralign-1);
		}
		section_header->sh_addr=*var;
		if (streq(string_table+section_header->sh_name,".module")){
			module->descriptor=(void*)(section_header->sh_addr);
		}
		else if (streq(string_table+section_header->sh_name,".gcov_info")){
			module->gcov_info.base=section_header->sh_addr;
			module->gcov_info.size=section_header->sh_size;
			INFO("Found .gcov_info section at %p (%v)",module->gcov_info.base,module->gcov_info.size);
		}
		if (section_header->sh_type==SHT_PROGBITS){
			memcpy((void*)(section_header->sh_addr),file_data+section_header->sh_offset,section_header->sh_size);
		}
		*var+=section_header->sh_size;
		section_header++;
	}
	if (!module->descriptor){
		panic("'.module' section not present");
	}
}



static void _resolve_symbol_table(void* file_data,const elf_hdr_t* header,elf_sym_t* symbol_table,u64 symbol_table_size,const char* string_table,const char* module_name){
	INFO("Resolving symbols...");
	_Bool unresolved_symbols=0;
	for (u64 i=0;i<symbol_table_size;i+=sizeof(elf_sym_t)){
		if (symbol_table->st_shndx==SHN_UNDEF&&string_table[symbol_table->st_name]){
			const symbol_t* symbol=symbol_lookup_by_name(string_table+symbol_table->st_name);
			if (symbol){
				symbol_table->st_value=symbol->rb_node.key;
			}
			else{
				ERROR("Unresolved symbol: %s",string_table+symbol_table->st_name);
				unresolved_symbols=1;
			}
		}
		else if (symbol_table->st_value){
			symbol_add(module_name,string_table+symbol_table->st_name,symbol_table->st_value+((const elf_shdr_t*)(file_data+header->e_shoff+symbol_table->st_shndx*sizeof(elf_shdr_t)))->sh_addr);
		}
		symbol_table++;
	}
	if (unresolved_symbols){
		panic("unresolved module symbols");
	}
}



static void _apply_relocations(void* file_data,const elf_hdr_t* header,const char* module_name){
	const elf_shdr_t* section_header=file_data+header->e_shoff;
	elf_sym_t* symbol_table=NULL;
	u64 symbol_table_size=0;
	const char* string_table=NULL;
	for (u16 i=0;i<header->e_shnum;i++){
		if (section_header->sh_type==SHT_SYMTAB){
			symbol_table=file_data+section_header->sh_offset;
			symbol_table_size=section_header->sh_size;
			section_header=file_data+header->e_shoff+section_header->sh_link*sizeof(elf_shdr_t);
			string_table=file_data+section_header->sh_offset;
			break;
		}
		section_header++;
	}
	_resolve_symbol_table(file_data,header,symbol_table,symbol_table_size,string_table,module_name);
	INFO("Applying relocations...");
	section_header=file_data+header->e_shoff;
	for (u16 i=0;i<header->e_shnum;i++){
		if (!section_header->sh_info||section_header->sh_info>=header->e_shnum){
			goto _skip_section;
		}
		u64 base=((const elf_shdr_t*)(file_data+header->e_shoff+section_header->sh_info*sizeof(elf_shdr_t)))->sh_addr;
		if (!base){
			goto _skip_section;
		}
		if (section_header->sh_type==SHT_REL){
			panic("SHT_REL");
		}
		else if (section_header->sh_type==SHT_RELA){
			const elf_rela_t* entry=file_data+section_header->sh_offset;
			for (u64 j=0;j<section_header->sh_size;j+=sizeof(elf_rela_t)){
				const elf_sym_t* symbol=symbol_table+(entry->r_info>>32);
				u64 relocation_address=base+entry->r_offset;
				u64 value=symbol->st_value;
				value+=entry->r_addend+((const elf_shdr_t*)(file_data+header->e_shoff+symbol->st_shndx*sizeof(elf_shdr_t)))->sh_addr;
				switch (entry->r_info&0xffffffff){
					case R_X86_64_NONE:
						break;
					case R_X86_64_64:
						*((u64*)relocation_address)=value;
						break;
					case R_X86_64_PC32:
					case R_X86_64_PLT32:
						*((u32*)relocation_address)=value-relocation_address;
						break;
					case R_X86_64_32:
					case R_X86_64_32S:
						*((u32*)relocation_address)=value;
						break;
					default:
						WARN("Unknon relocation type: %u",entry->r_info&0xffffffff);
						panic("Unknown relocation type");
				}
				entry++;
			}
		}
_skip_section:
		section_header++;
	}
}



module_t* module_load(const char* name){
	if (!name){
		return NULL;
	}
	LOG("Loading module '%s'...",name);
	spinlock_acquire_exclusive(&_module_global_lock);
	HANDLE_FOREACH(HANDLE_TYPE_MODULE){
		handle_acquire(handle);
		module_t* module=handle->object;
		if (streq(module->descriptor->name,name)){
			INFO("Module '%s' already loaded",name);
			handle_release(handle);
			spinlock_release_exclusive(&_module_global_lock);
			return module;
		}
		handle_release(handle);
	}
	vfs_node_t* directory=vfs_lookup(NULL,MODULE_ROOT_DIRECTORY,0);
	if (!directory){
		panic("Unable to find module root directory");
	}
	char buffer[256];
	SMM_TEMPORARY_STRING name_string=smm_alloc(buffer,format_string(buffer,256,"%s.mod",name));
	vfs_node_t* module_file=vfs_node_lookup(directory,name_string);
	if (!module_file){
		WARN("Unable to find module '%s'",name);
		spinlock_release_exclusive(&_module_global_lock);
		return NULL;
	}
	INFO("Loading module from file...");
	mmap_region_t* region=mmap_alloc(&(process_kernel->mmap),0,0,NULL,MMAP_REGION_FLAG_NO_FILE_WRITEBACK|MMAP_REGION_FLAG_VMM_NOEXECUTE|MMAP_REGION_FLAG_VMM_READWRITE,module_file);
	INFO("Module file size: %v",region->length);
	void* file_data=(void*)(region->rb_node.key);
	elf_hdr_t header=*((elf_hdr_t*)file_data);
	if (header.e_ident.signature!=0x464c457f||header.e_ident.word_size!=2||header.e_ident.endianess!=1||header.e_ident.header_version!=1||header.e_ident.abi!=0||header.e_type!=ET_REL||header.e_machine!=0x3e||header.e_version!=1){
		mmap_dealloc_region(&(process_kernel->mmap),region);
		spinlock_release_exclusive(&_module_global_lock);
		return NULL;
	}
	module_t* module=omm_alloc(&_module_allocator);
	module->state=MODULE_STATE_LOADING;
	handle_new(module,HANDLE_TYPE_MODULE,&(module->handle));
	_map_section_addresses(file_data,&header,module);
	_apply_relocations(file_data,&header,name);
	mmap_dealloc_region(&(process_kernel->mmap),region);
	INFO("Adjusting memory flags...");
	vmm_adjust_flags(&vmm_kernel_pagemap,module->ex_region.base,0,VMM_PAGE_FLAG_READWRITE,module->ex_region.size>>PAGE_SIZE_SHIFT);
	vmm_adjust_flags(&vmm_kernel_pagemap,module->nx_region.base,VMM_PAGE_FLAG_NOEXECUTE,VMM_PAGE_FLAG_READWRITE,module->nx_region.size>>PAGE_SIZE_SHIFT);
	vmm_adjust_flags(&vmm_kernel_pagemap,module->rw_region.base,VMM_PAGE_FLAG_NOEXECUTE,0,module->rw_region.size>>PAGE_SIZE_SHIFT);
	LOG("Module '%s' loaded successfully",name);
	spinlock_release_exclusive(&_module_global_lock);
	handle_finish_setup(&(module->handle));
	module->descriptor->init_callback(module);
	module->state=MODULE_STATE_LOADED;
	return module;
}
