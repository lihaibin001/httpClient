#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define BULK_EP_OUT     0x01
#define BULK_EP_IN      0x81

#define VENDER_ID 0x0483
#define PRODUCT_ID 0x5740

#define USB_DEBUG_ENABLE 1

#if USB_DEBUG_ENABLE
#define USB_DEBUG(...) do{ \
	USB_DEBUG("[USB] ")\
	USB_DEBUG(__VA_ARGS__) \
	}while(0);
#else
#define USB_DEBUG(...)
#endif

typedef struct
{
	uint8_t is_opened:1;
	uint8_t reserved:7;
}usb_status_falg_t;

static void usb_print_device_descriptors(struct libusb_device_descriptor desc)
{
	
	USB_DEBUG("Device Descriptors:\n-------------------\n");
	USB_DEBUG("Vendor ID: %04x\n", desc.idVendor);
	USB_DEBUG("Product ID: %04x\n", desc.idProduct);
	USB_DEBUG("Serial Number: %d\n", desc.iSerialNumber);
	USB_DEBUG("Size of Device Descriptor: %d\n", desc.bLength);
	USB_DEBUG("Type of Descriptor: %d\n", desc.bDescriptorType);
	USB_DEBUG("USB Specification Release Number: %d\n", desc.bcdUSB);
	USB_DEBUG("Device Release Number: %d\n", desc.bcdDevice);
	USB_DEBUG("Device Class: %d\n", desc.bDeviceClass);
	USB_DEBUG("Device Sub-Class: %d\n", desc.bDeviceSubClass);
	USB_DEBUG("Device Protocol: %d\n", desc.bDeviceProtocol);
	USB_DEBUG("Max. Packet Size: %d\n---------------------\n", desc.bMaxPacketSize0);
}

int usb_open(void)
{
	int r, cnt, i, found;
	struct libusb_device **devs;
	struct libusb_device_handle *handle = NULL, *hDevice_expected = NULL;
	struct libusb_device *dev, *dev_expected;

	struct libusb_device_descriptor desc;
	struct libusb_endpoint_descriptor *epdesc;
	struct libusb_interface_descriptor *intdesc;
	//Init libusb
	r = libusb_init(NULL);
	if(r < 0)
	{
		USB_DEBUG("Failed to initialize libusb. return: %d\n", r);
		return 1;
	}	
	
	//Get a list of USB device
	cnt = libusb_get_device_list(NULL, &devs);
	if(cnt < 0)
	{
		USB_DEBUG("There are no USB device on the bus\n");
		return 1	
	}
	USB_DEBUG("Device count: %d\n", cnt);

	while((dev = devs[i++]) != NULL)
	{
		r = libusb_device_descriptor(dev, &desc);
		if(r < 0)
		{
			USB_DEBUG("Failed to get device descriptor\n");
			libusb_free_device_list(devs, 1);
			return 1; 
		}
		
		r = libusb_open(dev, &handle);
		if(r < 0)
		{
			USB_DEBUG("Error opening device\n");
			libusb_free_device_list(devs, 1);
			if(handle != NULL)
			{
				libusb_close(handle);
			}
			return 1;
		}
		usb_print_device_descriptors(desc);
		
		if(desc.idVendor == VENDER_ID && desc.idProduct == PRODUCT_ID)
		{
			found = 1;
			break;
		}
	}
			
	if(found == 0)
	{
		USB_DEBUG("Device not found\n");
		libusb_free_device_list(devs, 1);
		libusb_close(handle);
		return 1;
	}
	else
	{
		USB_DEBUG("Device found");
		dev_expected = dev;
		hDevice_expected = handle;
	}
	libusb_free_device_list(devs,1);
	
	if(libusb_kernel_driver_active(handle, 0) == 1)
	{
		USB_DEBUG("Kenenl Driver active\n");
		if(libusb_detach_kernel_driver(handle, 0) == 0)
		{
			USB_DEBUG("Kernel driver detached\n");
		}
		else
		{
			USB_DEBUG("Couldn't detach kernel dirver\n");
			libusb_free_device_list(devs, 1);
			libusb_close(handle);
			return -1;	
		}
	}
	

	return 0;
}

transmite(uint8_t *pData, int len)
{
	return 0;
}

int usb_receive(uint8_t *pBuffer, int buffer_size)
{
	return 0;
