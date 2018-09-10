#include "kprotocal.h"
#include "cJSON/cJSON.h"
#include "ShareList.h"
static uint32_t sqnum = 1;
static uint32_t apid = 0x12345678;

typedef struct
{
	uint32_t ID;
	uint16_t Panid;
	uint8_t XD1;
	uint8_t XD2;
	uint8_t BACK1;
	uint8_t BACK2;
	uint8_t BACK3;
	uint8_t VER;
}tag_info_t;

typedef struct
{
	uint32_t ID;
	uint8_t LQI;
	uint8_t VOL;
	uint8_t TYPE;
	uint8_t FAC;
	uint8_t XD1;
	uint8_t XD2;
	uint8_t XD2;
	uint8_t VER;
}price_tag_t;

static tag_info_t tag_info;
static ShareList *pTag_list;


static uint32_t *encodeToJsonHeartbeat(uint8_t *pData)
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

static void decode_tag_info(uint8_t *pData)
{
	
}


static int decode_price_tag(uint8_t *pData)
{
	if(pTag_list == NULL)
	{
		pTag_list = SharedList_Create(sizeof(price_tag_t));	
		if(pTag_list == NULL)
		{
			return RET_MALLOC_ERR;
		}	
	}
		
}

int protocal_decode_usb_data(uint8_t *pData,uint32_t len)
{
	if(pData == NULL || len == 0)
	{
		return RET_BAD_PARA;	
	}
	if(pData[0] == 0x40)
	{
		decode_tag_info(pData);
	}
	else if(pData[0] == 0x44)
	{
		decode_price_tag(pData);		
	}
}

int protocal_encode_usb_resp(uint8_t *pEncodeData)
{
	if(pEncodeData == NULL)
	{
		return 	RET_BAD_PARA; 
	}
	
}

