#include "kprotocal.h"
#include "cjson/cJSON.h"
static uint32_t sqnum = 1;
#if 0
#pragma pack(1)
typedef struct
{
	uint8_t FUN;
	uint8_t LEN;
	uint32_t ID;
	uint16_t Panid;
	uint8_t XD1;
       	uint8_t XD2;
	uint8_t BACK1;
	uint8_t BACK2;
	uint8_t BACK3;
	uint8_t VER;
	uint8_t Mark;
	uint8_t CRCHI;
	uint8_t CRCLO;	
}status_with_no_label_req_t;
#pragma pack()

typedef struct
{
	uint8_t FUN;
	uint8_t LEN;
	uint8_t Wait;
	uint8_t CRCHI;
	uint8_t CRCLO;
}status_1_resp_t;

typedef struct
{
	uint8_t FUN;
	uint8_t LEN;
	uint8_t  
}
#endif
static uint32_t sunum;
uint8_t *protocalEncodeToJSONArray(uint8_t *pData)
{
	static cJSON *pItem = NULL;
	static cJSON *pBody = NULL;
	int8_t tmp[12] = "";
	if(pData == NULL)
	{
		return NULL;
	}	
        pItem = cJSON_CreateObject();
        pBody = cJSON_CreateArray();
        if(pItem == NULL)
        {
                printf("Create Json object error\n");
		return NULL;
        }
        sprintf(tmp, "%d", sqnum++);

        cJSON_AddStringToObject(pItem,"SEQUENCE",tmp);

        cJSON_AddStringToObject(pItem,"Function","64");
        cJSON_AddStringToObject(pItem,"APID","1234567890");
        cJSON_AddStringToObject(pItem,"XD1","02");
        cJSON_AddStringToObject(pItem,"XD2","03");
        cJSON_AddStringToObject(pItem,"BACK1","00");
        cJSON_AddStringToObject(pItem,"BACK2","00");
        cJSON_AddStringToObject(pItem,"BACK3","00");
        cJSON_AddStringToObject(pItem,"VER","00");

	cJSON_AddItemToArray(pBody,pItem);
	return cJSON_Print(pBody);
}

