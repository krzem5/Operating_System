#ifndef _KERNEL_USB_CONTROLLER_H_
#define _KERNEL_USB_CONTROLLER_H_ 1
#include <kernel/types.h>



typedef struct _USB_CONTROLLER{
	void* device;
	_Bool (*detect)(void*,u16);
	u8 (*reset)(void*,u16);
	void (*disconnect)(void*,u16);
} usb_controller_t;



usb_controller_t* usb_controller_alloc(void);



void usb_controller_dealloc(usb_controller_t* controller);


#endif
