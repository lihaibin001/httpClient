#include <pthread.h>
#include "usbCom.h"
#include <unistd.h>
#include "kprotocal.h"



void usbCom(void)                                                               
{                                                                               
    protocal_usb_porcessor();
}

void httpClent(void)
{	
    protocal_http_processor();
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

