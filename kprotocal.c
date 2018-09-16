#include "kprotocal.h"
#include <curl/curl.h>
#include "cJSON/cJSON.h"
#include "List.h"
#include <pthread.h>
#include "crc.h"
#include "string.h"
#include "usbCom.h"
#define PRICE_TAG_LEN 16
#define PROTOCAL_DEBUG_ENABLE 1

#if PROTOCAL_DEBUG_ENABLE
#define PROTOCAL_DEBUG(...) do{ \
    printf("[PROT] ");\
	printf(__VA_ARGS__); \
	}while(0)
#else
#define PROTOCAL_DEBUG(...)
#endif

#define VENDER_ID 0x0483
#define PRODUCT_ID 0x5740

static uint32_t sqnum;

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
    uint8_t BACK1;
    uint8_t BACK2;
    uint8_t BACK3;
    uint8_t VER;
}price_tag_t;

typedef struct
{
    Resp_type_t type;
    uint8_t *pTask;
}Cmd_t;

typedef struct
{
   uint8_t *pResp;
   uint8_t len;
}Resp_t;

static tag_info_t tag_info;
static List *pTag_list;
static List *pCmd_list;
static Resp_t Resp;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER; 
static uint32_t suqnum;

static bool lock_share_buff(void)
{
    pthread_mutex_trylock(&g_mutex);
    return true;
}

static bool unlock_share_buff(void)
{
    pthread_mutex_unlock(&g_mutex);
    return true;
}

static char *encode_to_tag_info_json(void)
{
    static cJSON *pItem = NULL;
    static cJSON *pBody = NULL;
    static cJSON *pHead = NULL;
    char *pCharData = NULL;
    pItem = cJSON_CreateArray();
    pBody = cJSON_CreateObject();
    pHead = cJSON_CreateObject();
    if(pItem == NULL || pBody == NULL || pHead == NULL)
    {
        PROTOCAL_DEBUG("Create Json object error\n");
        return NULL;
    }

    cJSON_AddNumberToObject(pHead, "Total",1);
    cJSON_AddNumberToObject(pHead,"SEQUENCE",sqnum++);

    cJSON_AddNumberToObject(pBody,"Function",0x40);
    cJSON_AddNumberToObject(pBody,"APID",tag_info.ID);
    cJSON_AddNumberToObject(pBody,"VER", tag_info.VER);
    cJSON_AddNumberToObject(pBody,"XD2", tag_info.XD2);
    cJSON_AddNumberToObject(pBody,"XD1", tag_info.XD1);
    cJSON_AddNumberToObject(pBody,"PANID", tag_info.Panid);
    cJSON_AddNumberToObject(pBody,"BACK1", tag_info.BACK1);
    cJSON_AddNumberToObject(pBody,"BACK2", tag_info.BACK2);
    cJSON_AddNumberToObject(pBody,"BACK3", tag_info.BACK3);

    cJSON_AddItemToArray(pItem,pHead);	
    cJSON_AddItemToArray(pItem,pBody);

    pCharData = cJSON_Print(pItem);
    cJSON_Delete(pItem);
    return pCharData;
}

static char *encode_to_tag_price_json(void)
{
    static cJSON *pItem = NULL;
    static cJSON *pBody = NULL;
    static cJSON *pHead = NULL;
    char *pCharData = NULL;
    uint8_t tag_cnt = List_Count(pTag_list), i;
    pItem = cJSON_CreateArray();
    if(pItem == NULL)
    {
        PROTOCAL_DEBUG("Create Item fail\n");
        return NULL;
    }
    pBody = cJSON_CreateObject();
    if(pBody == NULL)
    {
        cJSON_Delete(pItem);
        PROTOCAL_DEBUG("Create Item fail\n");
        return NULL;
        
    }
    pHead = cJSON_CreateObject();
    if(pHead == NULL)
    {
        if(pItem)
        {
            cJSON_Delete(pItem);
        }
        if(pBody)
        {
            cJSON_Delete(pItem);
        }
        PROTOCAL_DEBUG("Create Json object error\n");
        return NULL;
    }

    cJSON_AddNumberToObject(pHead, "Total",tag_cnt);
    cJSON_AddNumberToObject(pHead,"SEQUENCE",sqnum++);
    cJSON_AddItemToArray(pItem,pHead);	

    for(i=0; i<tag_cnt; i++)
    {
        price_tag_t *pTag = List_Find(pTag_list, i);
        cJSON_AddNumberToObject(pBody,"INDEX",i);
        cJSON_AddNumberToObject(pBody,"VER",pTag->VER);
        cJSON_AddNumberToObject(pBody,"VoL", pTag->VOL);
        cJSON_AddNumberToObject(pBody,"BACK3", pTag->BACK3);
        cJSON_AddNumberToObject(pBody,"LQI", pTag->LQI);
        cJSON_AddNumberToObject(pBody,"XD1", pTag->XD1);
        cJSON_AddNumberToObject(pBody,"Function", 0x44);
        cJSON_AddNumberToObject(pBody,"BACK2", pTag->BACK2);
        cJSON_AddNumberToObject(pBody,"PANID1", pTag->PANID1);
        cJSON_AddNumberToObject(pBody,"PANID2", pTag->PANID2);
        cJSON_AddNumberToObject(pBody,"BACK1", pTag->BACK1);
        cJSON_AddNumberToObject(pBody,"ECID", pTag->ID);
        cJSON_AddNumberToObject(pBody,"FAC", pTag->FAC);
        cJSON_AddNumberToObject(pBody,"TYPE", pTag->TYPE);
        cJSON_AddNumberToObject(pBody,"XD2", pTag->XD2);
        cJSON_AddItemToArray(pItem,pBody);
    }

    pCharData = cJSON_Print(pItem);
    cJSON_Delete(pItem);
    return pCharData;
    
}

static void decode_tag_info(uint8_t *pData)
{
        
}

static void prepack_price_tag(uint8_t *pData, price_tag_t *pPrice_tag)
{
    pPrice_tag->ID = (pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | pData[3];
    pPrice_tag->LQI = pData[4];
    pPrice_tag->VOL = pData[5];
    pPrice_tag->TYPE = pData[6];
    pPrice_tag->FAC = pData[7];
    pPrice_tag->XD1 = pData[8];
    pPrice_tag->PANID1 = (uint16_t)((pData[9] << 8) | pData[10]);
    pPrice_tag->XD2 = pData[11];
    pPrice_tag->PANID2 = (pData[12] << 8) | pData[12];
    pPrice_tag->VER = pData[17];
}


static int decode_price_tag(uint8_t *pData)
{
    int res = RET_FUNCTION_OK;
    uint8_t *pTmp = &pData[2];
    const price_tag_t *pTag = NULL;
    int tag_count = 0, i, tag_idx, tag_cnt;
    price_tag_t tag;
    tag_cnt = (pData[1] - 4) / PRICE_TAG_LEN; 
    lock_share_buff();
    for(tag_idx=0; tag_idx<tag_cnt; tag_idx++)
    {
        prepack_price_tag(&pTmp[tag_idx * PRICE_TAG_LEN], &tag);
        if(pTag_list == NULL)
        {
            pTag_list = List_Create(sizeof(price_tag_t));	
            if(pTag_list == NULL)
            {
                PROTOCAL_DEBUG("Create list error\n");
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
                    PROTOCAL_DEBUG("Error %d\n",__LINE__);
                    res = RET_GENERAL_ERR;
                }
            }

        }
        PROTOCAL_DEBUG("pirce tag count:%d\n", List_Count(pTag_list));
    }
    unlock_share_buff();
    return res;
}

int protocal_decode_usb_data(uint8_t *pData,uint32_t len)
{
    uint16_t crc = 0;
    if(pData == NULL || len == 0)
    {
        PROTOCAL_DEBUG("Parameter error%s,%d\n",__FILE__, __LINE__);
        return RET_BAD_PARA;	
    }
    crc = crc16_calc(pData, len-2);
    if(crc != ((pData[len - 2] << 8) | pData[len -1]))
    {
        PROTOCAL_DEBUG("CRC16 Error\n");
        return RET_CHECK_ERROR;
    }
    if(pData[0] == 0x40)
    {
        decode_tag_info(pData);
    }
    else if(pData[0] == 0x44)
    {
        decode_price_tag(pData);		
    }

    if(List_Count(pCmd_list) == 0)
    {
        protocal_encode_usb_resp((Resp_type_t)pData[0], pData); 
    }
    else
    {

    }
    return RET_FUNCTION_OK;
}


static int encode_0x40(void)
{
   Resp.pResp = malloc(5);
   if(Resp.pResp != NULL)
   {
       uint16_t crc = 0;
       Resp.pResp[0] = 0x40;
       Resp.pResp[1] = 5;
       Resp.pResp[2] = 0;
       crc = crc16_calc(Resp.pResp, 3);
       Resp.pResp[3] = (uint8_t)(crc >> 8);
       Resp.pResp[4] = (uint8_t)(crc);
       Resp.len = 5;
   }
   else
   {
       PROTOCAL_DEBUG("Malloc faile:%s,%d\n", __FILE__, __LINE__);
       return RET_MALLOC_ERR;
   }
   return RET_FUNCTION_OK;
}

static void encode_0x41(void)
{

}

static void encode_0x42(void)
{

}

static void encode_0x43(void)
{

}

static int encode_0x44(uint8_t *pSrc)
{
    uint8_t tag_cnt = (pSrc[1] - 4) / PRICE_TAG_LEN, i;
    uint16_t crc;
    Resp.len = tag_cnt * 4 + 5;
    Resp.pResp = malloc(Resp.len);
    if(Resp.pResp != NULL)
    {
        Resp.pResp[0] = 0x44;
        Resp.pResp[1] = tag_cnt * 4 + 5;
        for(i=0; i< tag_cnt; i++)
        {
            memcpy(&Resp.pResp[i*4+2], &pSrc[i*18+2], 4);
        }
        crc = crc16_calc(Resp.pResp, Resp.len-2);
        Resp.pResp[Resp.len-3] = 0;
        Resp.pResp[Resp.len-2] = (uint8_t)(crc>>8);
        Resp.pResp[Resp.len-1] = (uint8_t)crc;
    }
    else
    {
        return RET_MALLOC_ERR;
    }

    return RET_FUNCTION_OK;
}
int protocal_encode_usb_resp(Resp_type_t type, uint8_t *pSrc)
{
    switch(type)
    {
        case 0x40:
            return encode_0x40();
        case 0x41:
            break;
        case 0x42:
            break;
        case 0x43:
            break;
        case 0x44:
            return encode_0x44(pSrc);
        default:
            return RET_BAD_PARA;

    }
}

int protocal_send_usb_resp(void)
{
#if PROTOCAL_DEBUG_ENABLE
    PROTOCAL_DEBUG("Send Len:%d [ ",Resp.len);
    if(Resp.pResp)
    {   
        uint8_t i;
        for(i=0; i<Resp.len; i++)
        {
            printf("%02x ",Resp.pResp[i]);
        }
        printf("]\n");
    }
#endif
    int ret = usb_transmite(Resp.pResp, Resp.len, 1000);
    free(Resp.pResp);
    Resp.pResp = NULL;
    return ret;
}

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
	PROTOCAL_DEBUG("Http Receive: %s\n",pRxData->pData);
	PROTOCAL_DEBUG("[HTTP Client] Receive length: %d\n",pRxData->len);
	return pRxData->len;
}

void protocal_usb_porcessor(void)
{
    static uint8_t usb_receive_buff[2048];
	usb_open(VENDER_ID, PRODUCT_ID);
    int receive_len;
	for(;;)
	{
/*
		if(!usb_get_status())
		{
			usb_open(VENDER_ID, PRODUCT_ID);
		}
*/		
		if((receive_len = usb_receive(usb_receive_buff, 2048, 60000)) == 0)
		{
			continue;	
		}

        protocal_decode_usb_data(usb_receive_buff, receive_len); 
        protocal_send_usb_resp();
	}	
}

void protocal_http_processor(void)
{
    char *pPostData = NULL;
	RxBuffer_t RxBuffer;
	CURL *curl = NULL; 
	CURLcode res;
    for(;;)
    {
        if(List_Count(pTag_list) != 0)
        {
            pPostData = encode_to_tag_price_json();
        }
        else
        {
            pPostData = encode_to_tag_info_json();
        }
        if(pPostData == NULL)
        {
            continue;
        }
        PROTOCAL_DEBUG("Send: %s\n", pPostData);
    	curl = curl_easy_init();
        if(curl) 
        {
            //curl_easy_setopt(curl, CURLOPT_URL, "47.100.50.216:7095");
            curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.8:7095");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pPostData);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &RxBuffer);	
            res = curl_easy_perform(curl);
            if(res != CURLE_OK) 
            {
                for(;;)
                {
                    PROTOCAL_DEBUG("perform curl error:%d\n", res);
                }
            }
            else 
            {
                cJSON *root = cJSON_Parse((char *)RxBuffer.pData);
                if(root != NULL)
                {
                    cJSON *pTmp = cJSON_GetArrayItem(root, 0);
                    if(pTmp != NULL)
                    {
                        pTmp = cJSON_GetObjectItem(pTmp, "date");
                        if(pTmp != NULL)
                        {
                            PROTOCAL_DEBUG("Data: %s\n", pTmp->valuestring);
                            cJSON_Delete(root);
                            free(RxBuffer.pData);

                        }
                    }
                }
            }
            curl_easy_cleanup(curl);
        }
        free(pPostData);
        sleep(10);

    }
}

