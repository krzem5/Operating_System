import array
import struct
import sys



SHT_PROGBITS=1
SHT_SYMTAB=2
SHT_RELA=4

SHF_ALLOC=2

SHN_UNDEF=0

STT_FUNC=2
STT_SECTION=3
STT_FILE=4

STB_GLOBAL=1

STV_DEFAULT=0

R_X86_64_64=1
R_X86_64_PC32=2
R_X86_64_PLT32=4
R_X86_64_32=10
R_X86_64_32S=11



KERNEL_SECTION_ORDER=[".kernel_ue",".kernel_ur",".kernel_uw",".kernel_ex",".kernel_nx",".kernel_rw",".kernel_iw"]
KERNEL_START_ADDRESS=0xffffffffc0100000
KERNEL_EARLY_READ_ONLY_SECTION_NAME=".kernel_ur"



__all__=["link"]



class LinkerContext(object):
	def __init__(self,data):
		self.data=data
		self.section_headers={}
		self.section_headers_by_name={}
		self.symbol_table=None
		self.relocation_tables=[]
		self.relocation_entries=[]
		self.out=bytearray()

	def add_section_header(self,section_header):
		self.section_headers[section_header.index]=section_header
		self.section_headers_by_name[section_header.name]=section_header

	def set_symbol_table(self,symbol_table):
		self.symbol_table=symbol_table

	def add_relocation_table(self,relocation_table):
		self.relocation_tables.append(relocation_table)

	def add_relocation_entry(self,type,section,offset,symbol,addend):
		self.relocation_entries.append(RelocationEntry(type,section,offset,symbol,addend))



class SectionHeader(object):
	def __init__(self,index,name,offset,size,has_data):
		self.index=index
		self.name=name
		self.address=0
		self.offset=offset
		self.size=size
		self.has_data=has_data
		self.suffix_data=bytearray()



class SymbolTable(object):
	def __init__(self,offset,size,string_table_section):
		self.offset=offset
		self.size=size
		self.string_table_section=string_table_section
		self.symbols={}
		self.symbols_by_name={}

	def add_symbol(self,index,symbol):
		self.symbols[index]=symbol
		self.symbols_by_name[symbol.name]=symbol



class Symbol(object):
	def __init__(self,name,value,section,is_public,add_to_generated_table):
		self.name=name
		self.value=value
		self.section=section
		self.is_public=is_public
		self.add_to_generated_table=add_to_generated_table
		self.name_relocation_offset=None



class RelocationTable(object):
	def __init__(self,offset,size,target_section):
		self.offset=offset
		self.size=size
		self.target_section=target_section



class RelocationEntry(object):
	def __init__(self,type,section,offset,symbol,addend):
		self.type=type
		self.section=section
		self.offset=offset
		self.symbol=symbol
		self.addend=addend



def _parse_headers(data):
	out=LinkerContext(data)
	e_shoff,e_shentsize,e_shnum,e_shstrndx=struct.unpack("<40xQ10xHHH",data[:64])
	sh_name_offset=struct.unpack("<24xQ32x",data[e_shoff+e_shstrndx*e_shentsize:e_shoff+(e_shstrndx+1)*e_shentsize])[0]
	for i in range(0,e_shnum):
		sh_name,sh_type,sh_flags,sh_offset,sh_size,sh_link=struct.unpack("<IIQ8xQQI20x",data[e_shoff+i*e_shentsize:e_shoff+(i+1)*e_shentsize])
		name=data[sh_name_offset+sh_name:data.index(b"\x00",sh_name_offset+sh_name)].decode("utf-8")
		out.add_section_header(SectionHeader(i,name,sh_offset,sh_size,(sh_type==SHT_PROGBITS and (sh_flags&SHF_ALLOC))))
		if (sh_type==SHT_RELA):
			out.add_relocation_table(RelocationTable(sh_offset,sh_size,name.replace(".rela","")))
		elif (sh_type==SHT_SYMTAB):
			out.set_symbol_table(SymbolTable(sh_offset,sh_size,sh_link))
	return out



def _parse_symbol_table(ctx):
	st_name_offset=ctx.section_headers[ctx.symbol_table.string_table_section].offset
	for i in range(ctx.symbol_table.offset,ctx.symbol_table.offset+ctx.symbol_table.size,24):
		st_name,st_info,st_other,st_shndx,st_value=struct.unpack("<IBBHQ8x",ctx.data[i:i+24])
		name=ctx.data[st_name_offset+st_name:ctx.data.index(b"\x00",st_name_offset+st_name)].decode("utf-8")
		if ((st_shndx==SHN_UNDEF and not name.startswith("__KERNEL_SECTION_")) or (st_info&0x0f)==STT_FILE):
			continue
		is_func=((st_info&0x0f)==STT_FUNC)
		is_public=((st_info>>4)==STB_GLOBAL and st_other==STV_DEFAULT)
		if (name.startswith("__KERNEL_SECTION_")):
			is_func=False
			is_public=False
		ctx.symbol_table.add_symbol((i-ctx.symbol_table.offset)//24,Symbol(name,st_value,ctx.section_headers[st_shndx],is_public,is_func or is_public))



def _parse_relocation_tables(ctx):
	for relocation_table in ctx.relocation_tables:
		section=ctx.section_headers_by_name[relocation_table.target_section]
		if (not section.has_data):
			continue
		for i in range(relocation_table.offset,relocation_table.offset+relocation_table.size,24):
			r_offset,r_info,r_addend=struct.unpack("<QQq",ctx.data[i:i+24])
			ctx.add_relocation_entry(r_info&0xffffffff,section,r_offset,ctx.symbol_table.symbols[r_info>>32],r_addend)



def _generate_symbol_table(ctx):
	output_section=ctx.section_headers_by_name[KERNEL_EARLY_READ_ONLY_SECTION_NAME]
	for symbol in sorted(ctx.symbol_table.symbols.values(),key=lambda e:e.name):
		if (not symbol.add_to_generated_table):
			continue
		symbol.name_relocation_offset=output_section.size+len(output_section.suffix_data)
		output_section.suffix_data+=symbol.name.encode("utf-8")+b"\x00"
	output_section.suffix_data+=b"\x00"*((-len(output_section.suffix_data))&7)
	symbol_table_relocation_offset=output_section.size+len(output_section.suffix_data)
	for symbol in sorted(ctx.symbol_table.symbols.values(),key=lambda e:e.name):
		if (not symbol.add_to_generated_table):
			continue
		ctx.add_relocation_entry(R_X86_64_64,output_section,output_section.size+len(output_section.suffix_data),symbol,(not symbol.is_public)<<63)
		ctx.add_relocation_entry(R_X86_64_64,output_section,output_section.size+len(output_section.suffix_data)+8,Symbol("",symbol.name_relocation_offset,output_section,0,0),0)
		output_section.suffix_data+=struct.pack("<QQ",0,0)
	output_section.suffix_data+=struct.pack("<QQ",0,0)
	symbol=ctx.symbol_table.symbols_by_name["__kernel_symbol_data"]
	ctx.add_relocation_entry(R_X86_64_64,symbol.section,symbol.value,Symbol("",symbol_table_relocation_offset,output_section,0,0),0)



def _generate_relocation_table(ctx):
	output_section=ctx.section_headers_by_name[KERNEL_EARLY_READ_ONLY_SECTION_NAME]
	output_section.suffix_data+=b"\x00"*((-len(output_section.suffix_data))&7)
	relocation_table_relocation_offset=output_section.size+len(output_section.suffix_data)
	for relocation in ctx.relocation_entries[:]:
		width=0
		if (relocation.type==R_X86_64_64):
			width=8
		elif (relocation.type==R_X86_64_32 or relocation.type==R_X86_64_32S):
			width=4
		if (not width):
			continue
		ctx.add_relocation_entry(R_X86_64_64,output_section,output_section.size+len(output_section.suffix_data),Symbol("",relocation.offset,relocation.section,0,0),(width==8)<<63)
		output_section.suffix_data+=struct.pack("<Q",0)
	output_section.suffix_data+=struct.pack("<Q",0)
	symbol=ctx.symbol_table.symbols_by_name["__kernel_relocation_data"]
	ctx.add_relocation_entry(R_X86_64_64,symbol.section,symbol.value,Symbol("",relocation_table_relocation_offset,output_section,0,0),0)



def _place_sections(ctx):
	symbol=ctx.symbol_table.symbols_by_name[f"__KERNEL_SECTION_kernel_START__"]
	symbol.section=ctx.section_headers_by_name[KERNEL_SECTION_ORDER[0]]
	symbol.value=0
	for section_name in KERNEL_SECTION_ORDER:
		section=ctx.section_headers_by_name[section_name]
		section.address=KERNEL_START_ADDRESS+len(ctx.out)
		ctx.out+=ctx.data[section.offset:section.offset+section.size]+section.suffix_data
		ctx.out+=b"\x00"*((-len(ctx.out))&4095)
		section.size=(section.size+len(section.suffix_data)+4095)&(-4096)
		symbol=ctx.symbol_table.symbols_by_name[f"__KERNEL_SECTION_{section.name[1:]}_START__"]
		symbol.section=section
		symbol.value=0
		symbol=ctx.symbol_table.symbols_by_name[f"__KERNEL_SECTION_{section.name[1:]}_END__"]
		symbol.section=section
		symbol.value=section.size
	section=ctx.section_headers_by_name[KERNEL_SECTION_ORDER[-1]]
	symbol=ctx.symbol_table.symbols_by_name[f"__KERNEL_SECTION_kernel_END__"]
	symbol.section=section
	symbol.value=section.size



def _apply_relocations(ctx):
	for relocation in ctx.relocation_entries:
		relocation_address=relocation.section.address+relocation.offset
		relocation_value=relocation.symbol.section.address+relocation.symbol.value+relocation.addend
		output_offset=relocation_address-KERNEL_START_ADDRESS
		if (relocation.type==R_X86_64_64):
			ctx.out[output_offset:output_offset+8]=struct.pack("<Q",relocation_value&0xffffffffffffffff)
		elif (relocation.type==R_X86_64_PC32 or relocation.type==R_X86_64_PLT32):
			ctx.out[output_offset:output_offset+4]=struct.pack("<I",(relocation_value-relocation_address)&0xffffffff)
		elif (relocation.type==R_X86_64_32 or relocation.type==R_X86_64_32S):
			ctx.out[output_offset:output_offset+4]=struct.pack("<I",relocation_value&0xffffffff)
		else:
			print(f"Unknown relocation type '{relocation.type}'")
			sys.exit(1)



def link(src_file_path,dst_file_path):
	with open(src_file_path,"rb") as rf:
		data=bytearray(rf.read())
	ctx=_parse_headers(data)
	_parse_symbol_table(ctx)
	_parse_relocation_tables(ctx)
	_generate_symbol_table(ctx)
	_generate_relocation_table(ctx)
	_place_sections(ctx)
	_apply_relocations(ctx)
	with open(dst_file_path,"wb") as wf:
		wf.write(ctx.out)
	# quit()
