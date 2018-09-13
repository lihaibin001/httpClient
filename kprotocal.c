#include "kprotocal.h"
#include "cJSON/cJSON.h"
#include "List.h"
#include <pthread.h>

#define PROTOCAL_DEBUG_ENABLE 1

#if UPROTOCAL_DEBUG_ENABLE
#define LOCAL_DEBUG(...) do{ \
    printf("[PROT] ");\
	printf(__VA_ARGS__); \
	}while(0);
#else
#define LOCAL_DEBUG(...)
#endif

static uint32_t sqnum = 1;

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
    uint16_t PANID1;
    uint8_t XD2;
    uint16_t PANID2;
    uint8_t VER;
}price_tag_t;

static tag_info_t tag_info;
static List *pTag_list;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER; 
static uint32_t suqnum;

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
        LOCAL_DEBUG("Create Json object error\n");
        return NULL;
    }

    cJSON_AddNumberToObject(pHead, "Total",1);
    cJSON_AddNumberToObject(pHead,"SEQUENCE",sqnum++);

    cJSON_AddNumberToObject(pBody,"Function",0x40);
    cJSON_AddNumberToObject(pBody,"APID",tag_info.ID);
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


static bool lock_share_buff(void)
{
    pthread_mutex_trylock(&g_mutex);
}

static bool unlock_share_buff(void)
{
    pthread_mutex_unlock(&g_mutex);
}

static void decode_tag_info(uint8_t *pData)
{
    
}

static int prepack_price_tag(uint8_t *pData, price_tag_t *pPrice_tag)
{
    pPrice_tag->ID = (pData[2] << 24) || (pData[3] << 16) || (pData[4] << 8) || pData[5];
    pPrice_tag->LQI = pData[6];
    pPrice_tag->VOL = pData[7];
    pPrice_tag->TYPE = pData[8];
    pPrice_tag->FAC = pData[9];
    pPrice_tag->XD1 = pData[10];
    pPrice_tag->PANID1 = (pData[11] << 8) || pData[12];
    pPrice_tag->XD2 = pData[13];
    pPrice_tag->PANID2 = (pData[14] << 8) || pData[15];
    pPrice_tag->VER = pData[19];
}

static int decode_price_tag(uint8_t *pData)
{
    int res = RET_FUNCTION_OK;
    price_tag_t *pTag = NULL;
    int tag_count = 0, i;
    price_tag_t tag;
    prepack_price_tag(pData, &tag);
    lock_share_buff();
    if(pTag_list == NULL)
    {
        pTag_list = List_Create(sizeof(price_tag_t));	
        if(pTag_list == NULL)
        {
            res = RET_MALLOC_ERR;
        }	
    }
    if(pTag_list != NULL)
    {
        tag_count = List_Count(pTag_list);
        for(i=0; i<tag_count; i++)
        {
            pTag =  List_Find(pTag_list, i);
            if(pTag)
            {
                if(pTag->ID == tag.ID)
                {
                    List_Update(pTag_list, i, &tag);
                    break;
                }
            }
        }
        if(i == tag_count)
        {
            if(List_Add(pTag_list, &tag) != RET_FUNCTION_OK)
            {
                LOCAL_DEBUG("Error %d\n",__LINE__);
                res = RET_GENERAL_ERR;
            }
        }
        
    }
    unlock_share_buff();
    return res;
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

