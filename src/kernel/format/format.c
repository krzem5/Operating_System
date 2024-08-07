#include <kernel/format/format.h>
#include <kernel/types.h>



#define FLAG_SHORT_SHORT 1
#define FLAG_SHORT 2
#define FLAG_LONG 4
#define FLAG_SIGN 8
#define FLAG_HEX 16
#define MASK_SIZE (FLAG_SHORT_SHORT|FLAG_SHORT|FLAG_LONG)



typedef struct _FORMAT_BUFFER_STATE{
	char* buffer;
	u32 offset;
	u32 length;
} format_buffer_state_t;



static KERNEL_INLINE void _buffer_state_add(format_buffer_state_t* buffer_state,char c){
	buffer_state->buffer[buffer_state->offset]=c;
	buffer_state->offset+=(buffer_state->offset<buffer_state->length);
}



static KERNEL_INLINE char _format_base16_char(u8 value){
	value&=15;
	return value+(value>9?87:48);
}



static KERNEL_INLINE void _format_base10_int(u64 value,format_buffer_state_t* out){
	char buffer[20];
	u8 i=0;
	do{
		buffer[i]=(value%10)+48;
		i++;
		value/=10;
	} while (value);
	while (i){
		i--;
		_buffer_state_add(out,buffer[i]);
	}
}



static KERNEL_INLINE void _format_base16_int(u64 value,format_buffer_state_t* out){
	char buffer[16];
	u8 i=0;
	do{
		buffer[i]=_format_base16_char(value);
		i++;
		value>>=4;
	} while (value);
	while (i){
		i--;
		_buffer_state_add(out,buffer[i]);
	}
}



static void _format_int(__builtin_va_list* va,u8 flags,format_buffer_state_t* out){
	u64 data;
	if (flags&FLAG_SIGN){
		s64 signed_data=((flags&FLAG_LONG)?__builtin_va_arg(*va,s64):__builtin_va_arg(*va,s32));
		if ((flags&FLAG_SHORT)){
			signed_data=(s16)signed_data;
		}
		else if ((flags&FLAG_SHORT_SHORT)){
			signed_data=(s8)signed_data;
		}
		if (signed_data<0){
			_buffer_state_add(out,'-');
			signed_data=-signed_data;
		}
		data=signed_data;
	}
	else{
		data=((flags&FLAG_LONG)?__builtin_va_arg(*va,u64):__builtin_va_arg(*va,u32));
		if ((flags&FLAG_SHORT)){
			data=(u16)data;
		}
		else if ((flags&FLAG_SHORT_SHORT)){
			data=(u8)data;
		}
	}
	if (!data){
		_buffer_state_add(out,'0');
		return;
	}
	if (!(flags&FLAG_HEX)){
		_format_base10_int(data,out);
		return;
	}
	_format_base16_int(data,out);
}



KERNEL_PUBLIC u32 format_string(char* buffer,u32 length,const char* template,...){
	__builtin_va_list va;
	__builtin_va_start(va,template);
	u32 out=format_string_va(buffer,length,template,&va);
	__builtin_va_end(va);
	return out;
}



KERNEL_PUBLIC u32 format_string_va(char* buffer,u32 length,const char* template,__builtin_va_list* va){
	if (!length){
		return 0;
	}
	format_buffer_state_t out={
		buffer,
		0,
		length-1
	};
	while (*template){
		if (*template!='%'){
			_buffer_state_add(&out,*template);
			template++;
			continue;
		}
		u8 flags=0;
		while (1){
			template++;
			if (!*template){
				goto _end;
			}
			if (*template=='l'){
				flags=(flags&(~MASK_SIZE))|FLAG_LONG;
			}
			else if (*template=='h'&&*(template+1)=='h'){
				flags=(flags&(~MASK_SIZE))|FLAG_SHORT_SHORT;
				template++;
			}
			else if (*template=='h'){
				flags=(flags&(~MASK_SIZE))|FLAG_SHORT;
			}
			else{
				break;
			}
		}
		if (*template=='c'){
			_buffer_state_add(&out,__builtin_va_arg(*va,int));
		}
		else if (*template=='s'){
			const char* ptr=__builtin_va_arg(*va,const char*);
			if (!ptr){
				ptr="(null)";
			}
			while (*ptr){
				_buffer_state_add(&out,*ptr);
				ptr++;
			}
		}
		else if (*template=='d'){
			_format_int(va,flags|FLAG_SIGN,&out);
		}
		else if (*template=='u'){
			_format_int(va,flags,&out);
		}
		else if (*template=='x'){
			_format_int(va,flags|FLAG_HEX,&out);
		}
		else if (*template=='X'){
			u8 value=(u8)__builtin_va_arg(*va,u32);
			_buffer_state_add(&out,_format_base16_char(value>>4));
			_buffer_state_add(&out,_format_base16_char(value));
		}
		else if (*template=='v'){
			u64 size=__builtin_va_arg(*va,u64);
			if (size<0x400){
				_format_base10_int(size,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'B');
			}
			else if (size<0x100000){
				_format_base10_int((size+0x200)>>10,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'K');
				_buffer_state_add(&out,'B');
			}
			else if (size<0x40000000){
				_format_base10_int((size+0x80000)>>20,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'M');
				_buffer_state_add(&out,'B');
			}
			else if (size<0x10000000000ull){
				_format_base10_int((size+0x20000000)>>30,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'G');
				_buffer_state_add(&out,'B');
			}
			else if (size<0x4000000000000ull){
				_format_base10_int((size+0x8000000000ull)>>40,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'T');
				_buffer_state_add(&out,'B');
			}
			else if (size<0x1000000000000000ull){
				_format_base10_int((size+0x2000000000000ull)>>50,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'P');
				_buffer_state_add(&out,'B');
			}
			else{
				_format_base10_int((size+0x800000000000000ull)>>60,&out);
				_buffer_state_add(&out,' ');
				_buffer_state_add(&out,'E');
				_buffer_state_add(&out,'B');
			}
		}
		else if (*template=='p'){
			u64 address=__builtin_va_arg(*va,u64);
			u32 shift=64;
			while (shift){
				if (shift==32){
					_buffer_state_add(&out,'_');
				}
				shift-=4;
				_buffer_state_add(&out,_format_base16_char(address>>shift));
			}
		}
		else if (*template=='g'){
			const u8* uuid=__builtin_va_arg(*va,const u8*);
			for (u8 i=0;i<32;i++){
				if (i==8||i==12||i==16||i==20){
					_buffer_state_add(&out,'-');
				}
				_buffer_state_add(&out,_format_base16_char(uuid[i>>1]>>((!(i&1))<<2)));
			}
		}
		else if (*template=='M'){
			const u8* mac_address=__builtin_va_arg(*va,const u8*);
			for (u8 i=0;i<12;i++){
				if (i&&!(i&1)){
					_buffer_state_add(&out,':');
				}
				_buffer_state_add(&out,_format_base16_char(mac_address[i>>1]>>((!(i&1))<<2)));
			}
		}
		else if (*template=='I'){
			u32 ip4=__builtin_va_arg(*va,u32);
			for (u8 i=0;i<4;i++){
				if (i){
					_buffer_state_add(&out,'.');
				}
				_format_base10_int((ip4>>(24-(i<<3)))&0xff,&out);
			}
		}
		else if (*template=='j'){
			const u16* ip6=__builtin_va_arg(*va,const u16*);
			u8 contraction_index=0xff;
			u8 contraction_length=0;
			for (u8 i=0;i<8;i++){
				if (ip6[i]){
					continue;
				}
				u8 j=i+1;
				for (;j<8&&!ip6[j];j++);
				if (j-i>contraction_length){
					contraction_index=i;
					contraction_length=j-i;
				}
				i=j-1;
			}
			bool need_separator=0;
			for (u8 i=0;i<8;i++){
				if (i==contraction_index){
					_buffer_state_add(&out,':');
					_buffer_state_add(&out,':');
					i+=contraction_length-1;
					need_separator=0;
					continue;
				}
				if (need_separator){
					_buffer_state_add(&out,':');
				}
				_format_base16_int(ip6[i],&out);
				need_separator=1;
			}
		}
		else if (*template=='J'){
			const u16* ip6=__builtin_va_arg(*va,const u16*);
			for (u8 i=0;i<8;i++){
				if (i){
					_buffer_state_add(&out,':');
				}
				_format_base16_int(ip6[i],&out);
			}
		}
		else{
			_buffer_state_add(&out,'%');
			_buffer_state_add(&out,*template);
		}
		template++;
	}
_end:
	out.buffer[out.offset]=0;
	return out.offset;
}
