#include <stdio.h>                                                              
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
void usbCom(void)                                                               
{                                                                               
	for(;;)
	{
		printf("This is usb communication pthread.\n");
		sleep(1);
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

