#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

int usb_open(void);
int usb_transmite(uint8_t *pData, int len, int timeout);
int usb_receive(uint8_t *pBuffer, int buffer_size, int timeout);
bool usb_get_status(void);

