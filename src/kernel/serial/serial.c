#include <kernel/apic/ioapic.h>
#include <kernel/io/io.h>
#include <kernel/isr/isr.h>
#include <kernel/lock/spinlock.h>
#include <kernel/log/log.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "serial"



#define COM1_IRQ 4



static spinlock_t _serial_read_lock=SPINLOCK_INIT_STRUCT;
static spinlock_t _serial_write_lock=SPINLOCK_INIT_STRUCT;
static u8 KERNEL_INIT_WRITE _serial_irq=0;



void serial_init(void){
	io_port_out8(0x3f9,0x00);
	io_port_out8(0x3fb,0x80);
	io_port_out8(0x3f8,0x03);
	io_port_out8(0x3f9,0x00);
	io_port_out8(0x3fb,0x03);
	io_port_out8(0x3fa,0xc7);
	io_port_out8(0x3fc,0x03);
}



void serial_init_irq(void){
	LOG("Enabling serial IRQ...");
	_serial_irq=isr_allocate();
	INFO("Serial IRQ: %u",_serial_irq);
	ioapic_redirect_irq(COM1_IRQ,_serial_irq);
	io_port_out8(0x3f9,0x01);
	io_port_out8(0x3fc,0x0b);
}



void serial_send(const void* buffer,u32 length){
	scheduler_pause();
	spinlock_acquire_exclusive(&_serial_read_lock);
	for (;length;length--){
		SPINLOOP(!(io_port_in8(0x3fd)&0x20));
		io_port_out8(0x3f8,*((const u8*)buffer));
		buffer++;
	}
	spinlock_release_exclusive(&_serial_read_lock);
	scheduler_resume();
}



u32 serial_recv(void* buffer,u32 length,u64 timeout){
	spinlock_acquire_exclusive(&_serial_write_lock);
	u32 out=0;
	if (!timeout){
		for (;out<length;out++){
			while (!(io_port_in8(0x3fd)&0x01)){
				thread_await_event(IRQ_EVENT(_serial_irq));
			}
			*((u8*)buffer)=io_port_in8(0x3f8);
			buffer++;
		}
	}
	else{
		for (;out<length;out++){
			u64 i=timeout-1;
			do{
				__pause();
				i--;
			} while (i&&!(io_port_in8(0x3fd)&0x01));
			if (!i){
				break;
			}
			*((u8*)buffer)=io_port_in8(0x3f8);
			buffer++;
		}
	}
	spinlock_release_exclusive(&_serial_write_lock);
	return out;
}
