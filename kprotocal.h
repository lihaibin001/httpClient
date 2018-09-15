#ifndef __KPROTOCAL_H__
#define __KPROTOCAL_H__
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "CommonDefine.h"

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
*****@para      pBuffer           
*****@return    Retrun RET_FUNCTION_OK if no error 
**********************************/
int protocal_encode_usb_resp(uint8_t *pFuffer)
#endif // __KPROTOCAL_H__ 
