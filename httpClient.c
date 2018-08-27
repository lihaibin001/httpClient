#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "cJSON/cJSON.h"
#include <stdint.h>
#include "kprotocal.h"
#include <unistd.h>

typedef enum
{
	haveNoPriceLable,
	havePriceLable,
}routerStatus_t;

typedef struct
{
	uint8_t *pData;
	uint32_t len;
}RxBuffer_t;

static routerStatus_t status;
uint8_t normalInfo[] = {0x40, 0x11, 0x12, 0x1, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x04};

static size_t write_cb(char *buffer, size_t size, size_t nitems, void *userdata)
{
	RxBuffer_t *pRxData = (RxBuffer_t *)userdata;
	pRxData->len = (uint32_t) size * nitems;
	pRxData->pData = malloc(pRxData->len);
	if(pRxData->pData == NULL)
	{
		printf("malloc fail\n");
		return 0;
	}

	memcpy(pRxData->pData, buffer, pRxData->len);
	printf("[HTTP Client] Receive: %s\n",pRxData->pData);
	printf("[HTTP Client] Receive length: %d\n",pRxData->len);
	return pRxData->len;
}

int main(void)
{
	CURL *curl;
	CURLcode res;
	RxBuffer_t RxBuffer;
	uint8_t *pPostData = NULL;
	for(;;)
	{

		
		pPostData =  protocalEncodeToJSONArray(normalInfo);
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
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &RxBuffer);	
			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* check for errors */
			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			else {
				cJSON *root = cJSON_Parse((char *)RxBuffer.pData);
				//cJSON *root = cJSON_Parse("[{\"commit\":\"analysis success\",\"date\":\"2018-08-27 20:29:16\"}]");
				if(root == NULL)
				{
					printf("[HTTP Client] prase failed\n");
					return -1;
				}
				cJSON *pTmp = cJSON_GetArrayItem(root, 0);
				if(pTmp == NULL)
				{
					cJSON_Delete(root);
					printf("have no item of date\n");
					return -1;
				}
				pTmp = cJSON_GetObjectItem(pTmp, "date");
				if(pTmp == NULL)
				{
					cJSON_Delete(root);
					printf("have no item of date\n");
					return -1;
				}
				printf("Data: %s\n", pTmp->valuestring);
				cJSON_Delete(root);
				free(RxBuffer.pData);

			}
			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		free(pPostData);
		sleep(10);
	}
	return 0;
}
