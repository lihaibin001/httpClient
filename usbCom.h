#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>

/********************************* 
*****@function  Open the usb device
*****@para      vID vender id           
*****@para      pID product id             
*****@return    Return a 0 if opened sucessfully 
**********************************/
int usb_open(uint16_t vID, uint16_t pID);
/********************************* 
*****@function  Send data
*****@para      pData           
*****@para      len           
*****@para      timeout           
*****@return    Return a 0 if opened sucessfully 
**********************************/
int usb_transmite(uint8_t *pData, int len, int timeout);
/********************************* 
*****@function  Receive data
*****@para      pBuffer           
*****@para      buffer_size             
*****@para      timeout             
*****@return    Return the length of the received data 
**********************************/
int usb_receive(uint8_t *pBuffer, int buffer_size, int timeout);
/********************************* 
*****@function  Get usb deveice's status
*****@return    Return true of the usb device has been opened 
********************************/
bool usb_get_status(void);

