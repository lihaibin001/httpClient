#include "kprotocal.h"
#include "cJSON/cJSON.h"
static uint32_t sqnum = 1;
static uint32_t apid = 0x12345678;

static char  *encodeToJsonHeartbeat(uint8_t *pData)
{
	
	static cJSON *pItem = NULL;
	static cJSON *pBody = NULL;
	static cJSON *pHead = NULL;
	char *pCharData = NULL;
	uint32_t id;
	if(pData == NULL)
	{
		return NULL;
	}	
        pItem = cJSON_CreateArray();
        pBody = cJSON_CreateObject();
	pHead = cJSON_CreateObject();
        if(pItem == NULL || pBody == NULL || pHead == NULL)
        {
                printf("Create Json object error\n");
		return NULL;
        }
	
	cJSON_AddNumberToObject(pHead, "Total",1);
        cJSON_AddNumberToObject(pHead,"SEQUENCE",sqnum++);
	
        cJSON_AddNumberToObject(pBody,"Function",0x40);
        cJSON_AddNumberToObject(pBody,"APID",apid);
        cJSON_AddNumberToObject(pBody,"VER", (uint32_t)pData[13]);
        cJSON_AddNumberToObject(pBody,"XD2", (uint32_t)pData[9]);
        cJSON_AddNumberToObject(pBody,"XD1", (uint32_t)pData[8]);
        cJSON_AddNumberToObject(pBody,"PANID", (uint32_t)((pData[6] << 8) | pData[7]));
        cJSON_AddNumberToObject(pBody,"BACK1", (uint32_t)pData[10]);
        cJSON_AddNumberToObject(pBody,"BACK2", (uint32_t)pData[11]);
        cJSON_AddNumberToObject(pBody,"BACK3", (uint32_t)pData[12]);
	
	cJSON_AddItemToArray(pItem,pHead);	
	cJSON_AddItemToArray(pItem,pBody);

	pCharData = cJSON_Print(pItem);
	cJSON_Delete(pItem);
	return pCharData;
}

uint8_t *protocalEncodeToJSONArray(uint8_t *pData)
{
	if(pData == NULL)
	{
		return NULL;
	}
	switch((uint32_t)pData[0])
	{
		case 0x40:
			return encodeToJsonHeartbeat(pData);
			break;
		case 0x44:
			break;
		default:
			break;
	}	
	return NULL;
}

