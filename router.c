#include <pthread.h>
#include "usbCom.h"
#include <unistd.h>
#include "kprotocal.h"

#define VENDER_ID 0x0483
#define PRODUCT_ID 0x5740

static uint8_t usb_receive_buff[2048];
static uint8_t usb_transmite_buff[2048];

void usbCom(void)                                                               
{                                                                               
	usb_open(VENDER_ID, PRODUCT_ID);
	for(;;)
	{
		
		if(!usb_get_status())
		{
			usb_open(VENDER_ID, PRODUCT_ID);
		}
		
		if(usb_receive(usb_receive_buff, 2048, 60000) != 0)
		{
			continue;	
		}

	}	
}

void httpClent(void)
{	
	for(;;)
	{
		printf("This is http clent pthtread.\n");
		sleep(1);
	}
}

int main(void)                                                                  
{                                                                               
	pthread_t usbCommId;
	pthread_t httpClentId;     
	int i,ret;                                                                  
	ret=pthread_create(&usbCommId, NULL, (void *) usbCom,NULL);                          
	if(ret!=0){                                                                 
		printf ("Create pthread error!\n");                                     
		return 1;                                                               
	}                                                                           
	ret=pthread_create(&httpClentId, NULL, (void *) httpClent,NULL);                          
	if(ret!=0){                                                                 
		printf ("Create pthread error!\n");                                     
		return 1;                                                               
	}                                                                           
	pthread_join(usbCommId, NULL);                                                      
	pthread_join(httpClentId, NULL);                                                      
	return (0);                                                                 
}

