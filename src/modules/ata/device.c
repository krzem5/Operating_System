#include <ata/device.h>
#include <ata/registers.h>
#include <kernel/drive/drive.h>
#include <kernel/format/format.h>
#include <kernel/handle/handle.h>
#include <kernel/io/io.h>
#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/module/module.h>
#include <kernel/pci/pci.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "ata"



#define OPERATION_TIMEOUT 0xffffff



static pmm_counter_descriptor_t _ata_device_omm_pmm_counter=PMM_COUNTER_INIT_STRUCT("omm_ata_device");
static omm_allocator_t _ata_device_allocator=OMM_ALLOCATOR_INIT_STRUCT("ata_device",sizeof(ata_device_t),8,1,&_ata_device_omm_pmm_counter);



static KERNEL_INLINE void KERNEL_NOCOVERAGE _delay_400ns(const ata_device_t* device){
	io_port_in8(device->port+ATA_REG_DEV_CTL);
	io_port_in8(device->port+ATA_REG_DEV_CTL);
	io_port_in8(device->port+ATA_REG_DEV_CTL);
	io_port_in8(device->port+ATA_REG_DEV_CTL);
}



static _Bool KERNEL_NOCOVERAGE _wait_for_device(const ata_device_t* device,u8 mask,u8 value){
	u32 timeout=OPERATION_TIMEOUT;
	for (;timeout&&(io_port_in8(device->port+ATA_REG_STATUS)&mask)!=value;timeout--){
		if (io_port_in8(device->port+ATA_REG_STATUS)&STATUS_ERR){
			WARN("ATA/ATAPI device returned an error");
			return 0;
		}
		_delay_400ns(device);
	}
	return !!timeout;
}



static void _send_atapi_command(const ata_device_t* device,const u16* command,u16 return_length,u16* output_buffer){
	io_port_out8(device->port+ATA_REG_DRV_HEAD,0xa0|(device->is_slave<<4));
	_delay_400ns(device);
	io_port_out8(device->port+ATA_REG_ERROR,0x00);
	io_port_out8(device->port+ATA_REG_LBA1,return_length);
	io_port_out8(device->port+ATA_REG_LBA2,return_length>>8);
	io_port_out8(device->port+ATA_REG_COMMAND,ATA_CMD_PACKET);
	_delay_400ns(device);
	if (!_wait_for_device(device,STATUS_DRQ|STATUS_BSY,STATUS_DRQ)){
		return;
	}
	for (u8 i=0;i<6;i++){
		io_port_out16(device->port+ATA_REG_DATA,command[i]);
	}
	_delay_400ns(device);
	if (output_buffer){
		if (!_wait_for_device(device,STATUS_DRQ|STATUS_BSY,STATUS_DRQ)){
			return;
		}
		for (u8 i=0;i<(return_length>>1);i++){
			output_buffer[i]=io_port_in16(device->port+ATA_REG_DATA);
		}
	}
}



static u64 _ata_read_write(void* extra_data,u64 offset,void* buffer,u64 count){
	panic("_ata_read_write");
}



static u64 _atapi_read_write(void* extra_data,u64 offset,void* buffer,u64 count){
	const ata_device_t* device=extra_data;
	if (offset&DRIVE_OFFSET_FLAG_WRITE){
		WARN("ATA drives are read-only");
		return 0;
	}
	offset&=DRIVE_OFFSET_MASK;
	if (count>0xffffffff){
		count=0xffffffff;
	}
	u8 atapi_command[12]={
		ATAPI_CMD_READ_SECTORS,
		0x00,
		offset>>24,
		offset>>16,
		offset>>8,
		offset,
		count>>24,
		count>>16,
		count>>8,
		count,
		0x00,
		0x00
	};
	_send_atapi_command(device,(const u16*)atapi_command,2048,NULL);
	u16* out=buffer;
	for (u64 i=0;i<count;i++){
		_delay_400ns(device);
		if (!_wait_for_device(device,STATUS_BSY,0)){
			return 0;
		}
		u16 size=(io_port_in8(device->port+ATA_REG_LBA2)<<8)|io_port_in8(device->port+ATA_REG_LBA1);
		if (size!=2048){
			WARN("ATA drive returned sector not equal to 2048 bytes");
		}
		for (u16 j=0;j<2048;j+=2){
			*out=io_port_in16(device->port+ATA_REG_DATA);
			out++;
		}
	}
	if (!_wait_for_device(device,STATUS_DRQ|STATUS_BSY,0)){
		return 0;
	}
	return count;
}



static drive_type_t _ata_drive_type={
	"ATA",
	_ata_read_write
};
static drive_type_t _atapi_drive_type={
	"ATAPI",
	_atapi_read_write
};



static _Bool _ata_init(ata_device_t* device,u8 index){
	io_port_out8(device->port+ATA_REG_DEV_CTL,DEV_CTL_SRST);
	_delay_400ns(device);
	io_port_out8(device->port+ATA_REG_DEV_CTL,0);
	if (!_wait_for_device(device,STATUS_BSY,0)){
		return 0;
	}
	io_port_out8(device->port+ATA_REG_DRV_HEAD,0xa0|(device->is_slave<<4));
	_delay_400ns(device);
	io_port_out8(device->port+ATA_REG_LBA0,0);
	io_port_out8(device->port+ATA_REG_LBA1,0);
	io_port_out8(device->port+ATA_REG_LBA2,0);
	io_port_out8(device->port+ATA_REG_COMMAND,ATA_CMD_IDENTIFY);
	if (!io_port_in8(device->port+7)){
		return 0;
	}
	if (!_wait_for_device(device,STATUS_BSY,0)){
		return 0;
	}
	u16 signature=(io_port_in8(device->port+ATA_REG_LBA1)<<8)|io_port_in8(device->port+ATA_REG_LBA2);
	switch (signature){
		case 0x0000:
			device->is_atapi=0;
			break;
		case 0x14eb:
			io_port_out8(device->port+ATA_REG_COMMAND,ATA_CMD_ATAPI_IDENTIFY);
			if (!_wait_for_device(device,STATUS_BSY,0)){
				return 0;
			}
			device->is_atapi=1;
			break;
		case 0x3cc3:
		case 0x6996:
			return 0;
		default:
			WARN("Invalid ATA signature '%x'; ignoring drive",signature);
			return 0;
	}
	u16 buffer[256];
	for (u16 i=0;i<256;i++){
		buffer[i]=io_port_in16(device->port+ATA_REG_DATA);
	}
	if (!(buffer[49]&0x0200)){
		WARN("ATA/ATAPI drive does not support LBA; ignoring drive");
		return 0;
	}
	drive_config_t config={
		.type=(device->is_atapi?&_atapi_drive_type:&_ata_drive_type),
		.extra_data=device
	};
	format_string(config.name,DRIVE_NAME_LENGTH,"ata%u",index);
	bswap16_trunc_spaces(buffer+10,10,config.serial_number);
	bswap16_trunc_spaces(buffer+27,20,config.model_number);
	if (!device->is_atapi){
		WARN("Unimplemented: ATA drive");
		return 0;
	}
	const u8 atapi_command[12]={
		ATAPI_CMD_READ_CAPACITY,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};
	u8 output_buffer[8];
	_send_atapi_command(device,(const u16*)atapi_command,8,(u16*)output_buffer);
	config.block_count=((output_buffer[0]<<24)|(output_buffer[1]<<16)|(output_buffer[2]<<8)|output_buffer[3])+1;
	config.block_size=(output_buffer[4]<<24)|(output_buffer[5]<<16)|(output_buffer[6]<<8)|output_buffer[7];
	drive_create(&config);
	return 1;
}



static void _ata_init_device(pci_device_t* device){
	if (device->class!=0x01||device->subclass!=0x01||device->progif!=0x80){
		return;
	}
	pci_device_enable_memory_access(device);
	pci_device_enable_bus_mastering(device);
	pci_bar_t pci_bar;
	if (!pci_device_get_bar(device,4,&pci_bar)){
		return;
	}
	LOG("Attached ATA driver to PCI device %x:%x:%x",device->address.bus,device->address.slot,device->address.func);
	for (u8 i=0;i<4;i++){
		ata_device_t* ata_device=omm_alloc(&_ata_device_allocator);
		ata_device->dma_address=(void*)(pci_bar.address);
		ata_device->is_slave=i&1;
		ata_device->port=((i>>1)?0x170:0x1f0);
		if (!_ata_init(ata_device,i)){
			omm_dealloc(&_ata_device_allocator,ata_device);
		}
	}
}



void ata_locate_devices(void){
	drive_register_type(&_ata_drive_type);
	drive_register_type(&_atapi_drive_type);
	HANDLE_FOREACH(HANDLE_TYPE_PCI_DEVICE){
		pci_device_t* device=handle->object;
		_ata_init_device(device);
	}
}