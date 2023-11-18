#include <sys/fd.h>
#include <sys/io.h>
#include <sys/memory.h>
#include <sys/types.h>
#include <linker/search_path.h>
#include <linker/shared_object.h>
#include <linker/symbol.h>



static shared_object_t* _shared_object_tail=NULL;

shared_object_t* shared_object_root=NULL;



static void* memcpy(void* dst,const void* src,u64 length){
	u8* dst_ptr=dst;
	const u8* src_ptr=src;
	for (u64 i=0;i<length;i++){
		dst_ptr[i]=src_ptr[i];
	}
	return dst;
}



static shared_object_t* _alloc_shared_object(u64 image_base){
	static shared_object_t buffer[16];
	static u8 index=0;
	shared_object_t* out=buffer+(index++);
	out->next=NULL;
	out->image_base=image_base;
	if (_shared_object_tail){
		_shared_object_tail->next=out;
	}
	else{
		shared_object_root=out;
	}
	_shared_object_tail=out;
	return out;
}



shared_object_t* shared_object_init(u64 image_base,const elf_dyn_t* dynamic_section){
	shared_object_t* so=_alloc_shared_object(image_base);
	if (!dynamic_section){
		return so;
	}
	so->dynamic_section.has_needed_libraries=0;
	so->dynamic_section.plt_relocation_size=0;
	so->dynamic_section.plt_got=NULL;
	so->dynamic_section.hash_table=NULL;
	so->dynamic_section.string_table=NULL;
	so->dynamic_section.symbol_table=NULL;
	so->dynamic_section.relocations=NULL;
	so->dynamic_section.relocation_size=0;
	so->dynamic_section.relocation_entry_size=0;
	so->dynamic_section.symbol_table_entry_size=0;
	so->dynamic_section.plt_relocation_entry_size=0;
	so->dynamic_section.plt_relocations=NULL;
	for (const elf_dyn_t* dyn=dynamic_section;dyn->d_tag!=DT_NULL;dyn++){
		switch (dyn->d_tag){
			case DT_NEEDED:
				so->dynamic_section.has_needed_libraries=1;
				break;
			case DT_PLTRELSZ:
				so->dynamic_section.plt_relocation_size=dyn->d_un.d_val;
				break;
			case DT_PLTGOT:
				so->dynamic_section.plt_got=so->image_base+dyn->d_un.d_ptr;
				break;
			case DT_HASH:
				so->dynamic_section.hash_table=so->image_base+dyn->d_un.d_ptr;
				break;
			case DT_STRTAB:
				so->dynamic_section.string_table=so->image_base+dyn->d_un.d_ptr;
				break;
			case DT_SYMTAB:
				so->dynamic_section.symbol_table=so->image_base+dyn->d_un.d_ptr;
				break;
			case DT_RELA:
				so->dynamic_section.relocations=so->image_base+dyn->d_un.d_ptr;
				break;
			case DT_RELASZ:
				so->dynamic_section.relocation_size=dyn->d_un.d_val;
				break;
			case DT_RELAENT:
				so->dynamic_section.relocation_entry_size=dyn->d_un.d_val;
				break;
			case DT_SYMENT:
				so->dynamic_section.symbol_table_entry_size=dyn->d_un.d_val;
				break;
			case DT_PLTREL:
				so->dynamic_section.plt_relocation_entry_size=(dyn->d_un.d_val==DT_RELA?sizeof(elf_rela_t):sizeof(elf_rel_t));
				break;
			case DT_JMPREL:
				so->dynamic_section.plt_relocations=so->image_base+dyn->d_un.d_ptr;
				break;
		}
	}
	if (!so->dynamic_section.string_table||!so->dynamic_section.hash_table||!so->dynamic_section.symbol_table||!so->dynamic_section.symbol_table_entry_size){
		so->dynamic_section.hash_table=NULL;
	}
	if (so->dynamic_section.plt_got){
		so->dynamic_section.plt_got[1]=(u64)so;
		so->dynamic_section.plt_got[2]=(u64)symbol_resolve_plt_trampoline;
	}
	if (so->dynamic_section.has_needed_libraries&&so->dynamic_section.string_table){
		for (const elf_dyn_t* dyn=dynamic_section;dyn->d_tag!=DT_NULL;dyn++){
			if (dyn->d_tag==DT_NEEDED){
				shared_object_load(so->dynamic_section.string_table+dyn->d_un.d_val);
			}
		}
	}
	if (so->dynamic_section.relocations&&so->dynamic_section.relocation_size&&so->dynamic_section.relocation_entry_size){
		for (u64 i=0;i<so->dynamic_section.relocation_size;i+=so->dynamic_section.relocation_entry_size){
			const elf_rela_t* relocation=so->dynamic_section.relocations+i;
			const elf_sym_t* symbol=so->dynamic_section.symbol_table+(relocation->r_info>>32)*so->dynamic_section.symbol_table_entry_size;
			switch (relocation->r_info&0xffffffff){
				case R_X86_64_COPY:
					memcpy((void*)(so->image_base+relocation->r_offset),(void*)symbol_lookup_by_name(so->dynamic_section.string_table+symbol->st_name),symbol->st_size);
					break;
				case R_X86_64_GLOB_DAT:
					*((u64*)(so->image_base+relocation->r_offset))=symbol_lookup_by_name(so->dynamic_section.string_table+symbol->st_name);
					break;
				case R_X86_64_RELATIVE:
					*((u64*)(so->image_base+relocation->r_offset))=so->image_base+relocation->r_addend;
					break;
				default:
					printf("Unknown relocation type: %u\n",relocation->r_info&0xffffffff);
					return NULL;
			}
		}
	}
	return so;
}



shared_object_t* shared_object_load(const char* name){
	char buffer[4096];
	s64 fd=search_path_find_library(name,buffer,4096);
	if (!fd){
		printf("Unable to find library '%s'\n",name);
		return NULL;
	}
	const void* base_file_address=sys_memory_map(0,SYS_MEMORY_FLAG_NOWRITEBACK|SYS_MEMORY_FLAG_FILE|SYS_MEMORY_FLAG_READ,fd);
	sys_fd_close(fd);
	const elf_hdr_t* header=base_file_address;
	if (header->e_ident.signature!=0x464c457f||header->e_ident.word_size!=2||header->e_ident.endianess!=1||header->e_ident.header_version!=1||header->e_ident.abi!=0||header->e_type!=ET_DYN||header->e_machine!=0x3e||header->e_version!=1){
		return NULL;
	}
	u64 max_address=0;
	for (u16 i=0;i<header->e_phnum;i++){
		const elf_phdr_t* program_header=(void*)(base_file_address+header->e_phoff+i*header->e_phentsize);
		if (program_header->p_type!=PT_LOAD){
			continue;
		}
		u64 address=program_header->p_vaddr+program_header->p_memsz;
		if (address>max_address){
			max_address=address;
		}
	}
	max_address=(max_address+4095)&0xfffffffffffff000ull;
	void* image_base=sys_memory_map(max_address,SYS_MEMORY_FLAG_WRITE,0);
	const elf_dyn_t* dynamic_section=NULL;
	for (u16 i=0;i<header->e_phnum;i++){
		const elf_phdr_t* program_header=(void*)(base_file_address+header->e_phoff+i*header->e_phentsize);
		if (program_header->p_type==PT_DYNAMIC){
			dynamic_section=image_base+program_header->p_vaddr;
			continue;
		}
		if (program_header->p_type!=PT_LOAD){
			continue;
		}
		u64 flags=0;
		if (program_header->p_flags&PF_R){
			flags|=SYS_MEMORY_FLAG_READ;
		}
		if (program_header->p_flags&PF_W){
			flags|=SYS_MEMORY_FLAG_WRITE;
		}
		if (program_header->p_flags&PF_X){
			flags|=SYS_MEMORY_FLAG_EXEC;
		}
		memcpy(image_base+program_header->p_vaddr,base_file_address+program_header->p_offset,program_header->p_filesz);
		sys_memory_change_flags(image_base+program_header->p_vaddr,program_header->p_memsz,flags);
	}
	sys_memory_unmap((void*)base_file_address,0);
	return shared_object_init((u64)image_base,dynamic_section);
}