#ifndef __KPROTOCAL_H__
#define __KPROTOCAL_H__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CommonDefine.h"

typedef enum
{
    Resp_No_Cmd = 0x40,
    Resp_Setting_Cmd = 0x41,
    Resp_Quick_Modify_Map_Cmd = 0x42,
    Resp_Map_Data_Cmd = 0x43,
    Resp_Common_Cmd = 0x44,
}Resp_type_t;

/********************************* 
*****@function  Encode data to JSON arrary 
*****@para      pData           
*****@return    Return a pointer point to the encoded data 
**********************************/
uint8_t * protocalEncodeToJSONArray(uint8_t *pData);
/********************************* 
*****@function  Decode data receive from usb device 
*****@para      pData           
*****@return    Return a pointer point to the encoded data 
**********************************/
int protocal_decode_usb_data(uint8_t *pData,uint32_t len);
/********************************* 
*****@function  Encode response
*****@para      type           
*****@return    Retrun RET_FUNCTION_OK if no error 
**********************************/
int protocal_encode_usb_resp(Resp_type_t  type, uint8_t *pSrc);
/********************************* 
*****@function Send respose
*****@return    Retrun RET_FUNCTION_OK if no error 
**********************************/
int protocal_send_usb_resp(void);
/********************************* 
*****@function  Used to handle usb device data 
*****@return    Retrun RET_FUNCTION_OK if no error 
**********************************/
void protocal_usb_porcessor(void);
/********************************* 
*****@function  Used to handle http data 
*****@return    Retrun RET_FUNCTION_OK if no error 
**********************************/
void protocal_http_processor(void);
#endif // __KPROTOCAL_H__ 
