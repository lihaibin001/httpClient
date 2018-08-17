#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <stdint.h>
#include "kprotocal.h"
#include <unistd.h>
uint8_t testData[] = {0x40, 0x11, 0x12, 0x1, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04};

int main(void)
{
	CURL *curl;
	CURLcode res;
	uint8_t *pPostData = NULL;
	for(;;)
	{


		pPostData =  protocalEncodeToJSONArray(testData);
		if(pPostData == NULL)
		{
			continue;
		}
		printf("%s\n",pPostData);
		curl = curl_easy_init();
		if(curl) {
			/* First set the URL that is about to receive our POST. This URL can
			   just as well be a https:// URL if that is what should receive the
			   data. */
			curl_easy_setopt(curl, CURLOPT_URL, "47.100.50.216:7095");
			/* Now specify the POST data */
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostData);
			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		sleep(1);
	}
	return 0;
}
