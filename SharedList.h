#ifndef __SHAREDLIST_H__
#define __SHAREDLIST_H__
#include "List.h"

//队列结构体
typedef struct SharedList
{
	List* pList;
	volatile int op_code;
}SharedList;


/*********************************
*****@function Create a share list
*****@para      nItemLen express the lenthg of the item
*****@return    A pointer ponit to the created share list
**********************************/
SharedList* SharedList_Create(int nItemLen);

/*********************************
*****@function  Destroy the express list
*****@para      pList express the list to be destoried
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int SharedList_Destroy(SharedList* pList);

/*********************************
*****@function  add a item to the express list
*****@para      pList
*****@para      pItem
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int SharedList_Add(SharedList* pList, void* pItem);


/*********************************
*****@function  Get the data's copy by it's index 
*****@para      pList
*****@para      nIndex
*****@return    return a pointer point the data's copy
**********************************/
void* SharedList_FindCopy(SharedList* pList, int nIndex);

/*********************************
*****@function  Remove a item by it's index
*****@para      pList
*****@para      nIndex
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int SharedList_Remove(SharedList* pList, int nIndex);

/*********************************
*****@function  updata the item in the list
*****@para      pList
*****@para      nIndex
*****@para      pItem  
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int ShareList_update(ShareList *pList, int nIndex, void *pItem);

/*********************************
*****@function  Handle the item in the list 
*****@para      pList
*****@para      nIndex
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int SharedList_Iterator(SharedList* pList, IteratorHandler handler);

/*********************************
*****@function Get the count of item int the list 
*****@para      pList
*****@return    return the count
**********************************/
int SharedList_Count(SharedList* pList);

/*********************************
*****@function  lock the list
*****@para      pList
*****@return    return RET_FUNCTION_OK if opreate sucessfully
**********************************/
int SharedList_Lock(SharedList* pList);

/*********************************
*****@function  ulock the list 
*****@para      pList
*****@return    
**********************************/
int SharedList_Unlock(SharedList* pList);
#endif
