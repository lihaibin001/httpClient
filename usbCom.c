#include "usbCom.h"
#include <sys/types.h>

#define BULK_EP_OUT     0x01
#define BULK_EP_IN      0x81


#define USB_DEBUG_ENABLE 1

#if USB_DEBUG_ENABLE
#define USB_DEBUG(...) do{ \
	printf("[USB] "); \
	printf(__VA_ARGS__); \
	}while(0);
#else
#define USB_DEBUG(...)
#endif

typedef struct
{
	uint8_t is_opened:1;
	uint8_t reserved:7;
}usb_status_falg_t;

int interface_number = -1;
usb_status_falg_t usb_status_falg;
static struct libusb_device_handle *gHandle;

int print_configuration(struct libusb_device_handle *hDevice, struct libusb_config_descriptor *config)
{
	char *data;
	int index;

	data = (char *)malloc(512);
	memset(data, 0, 512);

	index = config->iConfiguration;

	libusb_get_string_descriptor_ascii(hDevice, index, data, 512);

	USB_DEBUG("Interface Descriptors: ");
	USB_DEBUG("Number of Interfaces: %d", config->bNumInterfaces);
	USB_DEBUG("Length: %d", config->bLength);
	USB_DEBUG("Desc_Type: %d", config->bDescriptorType);
	USB_DEBUG("Config_index: %d", config->iConfiguration);
	USB_DEBUG("Total length: %lu", config->wTotalLength);
	USB_DEBUG("Configuration Value: %d", config->bConfigurationValue);
	USB_DEBUG("Configuration Attributes: %d", config->bmAttributes);
	USB_DEBUG("MaxPower(mA): %d\n", config->MaxPower);

	free(data);
	data = NULL;
	return 0;
}
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

static const struct libusb_endpoint_descriptor* active_config(struct libusb_device *pDev, struct libusb_device_handle *pHandle)
{
	struct libusb_config_descriptor *config;
	const struct libusb_endpoint_descriptor *endpoint;
	int altsetting_index, interface_index=0, ret_active;
	int i, ret_print;


	ret_active = libusb_get_active_config_descriptor(pDev, &config);
	ret_print = print_configuration(pHandle, config);

	for (interface_index=0;interface_index<config->bNumInterfaces;interface_index++)
	{
		const struct libusb_interface *iface = &config->interface[interface_index];
		for (altsetting_index=0; altsetting_index<iface->num_altsetting; altsetting_index++)
		{
			const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_index];

			int endpoint_index;
			for(endpoint_index=0; endpoint_index<altsetting->bNumEndpoints; endpoint_index++)
			{
				endpoint = &altsetting->endpoint[endpoint_index];
                USB_DEBUG("Endpoint desc:----------------------------------\n");
				USB_DEBUG("Size of EndPoint Descriptor: %d\n", endpoint->bLength);
				USB_DEBUG("Type of EndPoint Descriptor: %d\n", endpoint->bDescriptorType);
				USB_DEBUG("Endpoint Address: 0x%x\n", endpoint->bEndpointAddress);
				USB_DEBUG("Maximum Packet Size: %d\n", endpoint->wMaxPacketSize);
				USB_DEBUG("Attributes applied to Endpoint: %d\n", endpoint->bmAttributes);
				USB_DEBUG("Interval for Polling for data Tranfer: %d\n-------------------------------------\n", endpoint->bInterval);
			}
		}
	}
	libusb_free_config_descriptor(NULL);
	return endpoint;
}

int usb_open(uint16_t vID, uint16_t pID)
{
	int r, cnt, i, found;
	struct libusb_device **devs;
	struct libusb_device_handle *hDevice_expected = NULL;
	struct libusb_device *dev, *dev_expected;

	struct libusb_device_descriptor desc;
	struct libusb_endpoint_descriptor *epdesc;
	struct libusb_interface_descriptor *intdesc;
	//Init libusb
	r = libusb_init(NULL);
	if(r < 0)
	{
		USB_DEBUG("Failed to initialize libusb. return: %d\n", r);
		return -1;
	}	
	
	//Get a list of USB device
	cnt = libusb_get_device_list(NULL, &devs);
	if(cnt < 0)
	{
		USB_DEBUG("There are no USB device on the bus\n");
		return -1;
	}
	USB_DEBUG("Device count: %d\n", cnt);

	while((dev = devs[i++]) != NULL)
	{
		r = libusb_get_device_descriptor(dev, &desc);
		if(r < 0)
		{
			USB_DEBUG("Failed to get device descriptor\n");
			libusb_free_device_list(devs, 1);
			return -1; 
		}
		
		r = libusb_open(dev, &gHandle);
		if(r < 0)
		{
			USB_DEBUG("Error opening device\n");
			libusb_free_device_list(devs, 1);
			libusb_close(gHandle);
			return -1;
		}
		usb_print_device_descriptors(desc);
		
		if(desc.idVendor == vID && desc.idProduct == pID)
		{
			found = 1;
            break;
		}
	}
			
	if(found == 0)
	{
		USB_DEBUG("Device not found\n");
		libusb_free_device_list(devs, 1);
		libusb_close(gHandle);
		return -1;
	}
	else
	{
		USB_DEBUG("Device found\n");
		//dev_expected = dev;
		//hDevice_expected = handle;
	}
	libusb_free_device_list(devs,1);
	
	if(libusb_kernel_driver_active(gHandle, 0) == 1)
	{
		USB_DEBUG("Kenenl Driver active\n");
		if(libusb_detach_kernel_driver(gHandle, 0) == 0)
		{
			USB_DEBUG("Kernel driver detached\n");
		}
		else
		{
			USB_DEBUG("Couldn't detach kernel dirver\n");
			libusb_free_device_list(devs, 1);
			libusb_close(gHandle);
			return -1;	
		}
	}
	r = libusb_claim_interface(gHandle, 0);
    if(r < 0)
    {
        USB_DEBUG("Claim interface error\n");
		libusb_free_device_list(devs, 1);
		libusb_close(gHandle);
        return -1;
    }
    usb_status_falg.is_opened = 1;
	active_config(dev, gHandle);

	
	return 0;
}


int usb_transmite(uint8_t *pData, int len, int timeout)
{
    int transmited = 0;
    while(len)
    {
        if(0 != libusb_bulk_transfer(gHandle, BULK_EP_OUT, pData, 64, &transmited, timeout)) 
        {
            return 1;
        }
        pData += 64;
        len -= 64;
    }
    if(len != 0)
    {
        if(0 !=libusb_bulk_transfer(gHandle, BULK_EP_OUT, pData, len, &transmited, timeout))
        {
            return 1;
        }
    }
	return 0;
}

int usb_receive(uint8_t *pBuffer, int buffer_size, int timeout)
{
    if(pBuffer == NULL || buffer_size == 0)
    {
        return 0;
    }
    int received = 0;
    int r = 0;
    do
    {
        if(0 != libusb_bulk_transfer(gHandle, BULK_EP_IN, &pBuffer[r], 64, &received, timeout)) 
        {
            return 1;
        }
        r += received;
        buffer_size -= received;
    }while(received == 64 && buffer_size > 0);
#if USB_DEBUG_ENABLE
    {
        int i;
        USB_DEBUG("Recieve length:%d: [", r);
        for(i=0; i<r; i++)
        {
            printf("%02x ",pBuffer[i]);
        }
        printf("]\n");
    }
#endif
	return r;
}

bool usb_get_status(void)
{
    return usb_status_falg.is_opened;
}

