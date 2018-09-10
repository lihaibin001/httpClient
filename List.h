#ifndef __LIST_H__
#define __LIST_H__

typedef int IteratorHandler(void*);
typedef struct ListItem
{
	void* pItem;
	struct ListItem* pNext;
}ListItem;
typedef struct List 
{
	int itemLen;
	int count;
	ListItem* pHead;
	ListItem* pBufferHead;
	int bufferCnt;
}List;


/********************************* 
*****@function  
*****@para      nItemLen                     
*****@return    
**********************************/
List* List_Create(int nItemLen);

/********************************* 
*****@function  
*****@para      pList           
*****@return    
**********************************/
int List_Destroy(List* pList);

/********************************* 
*****@function  
*****@para      pList           
*****@para      pItem           
*****@return    
**********************************/
int List_Add(List* pList, void* pItem);


/********************************* 
*****@function  
*****@para      pList           
*****@para      nIndex          
*****@return    
**********************************/
const void* List_Find(List* pList, int nIndex);

/********************************* 
*****@function  
*****@para      pList           
*****@para      nIndex          
*****@para      nItem          
*****@return    
**********************************/
int List_Update(List *pList, int nIndex, void *pItem);

/********************************* 
*****@function  
*****@para      pList          
*****@para      nIndex         
*****@return    
**********************************/
void* List_FindCopy(List* pList, int nIndex);

/********************************* 
*****@function  
*****@para      pList           
*****@para      nIndex          
*****@return    
**********************************/
int List_Remove(List* pList, unsigned int nIndex);

/*********************************
*****@function  
*****@para      pList           
*****@para      nIndex         
*****@return    
**********************************/
int List_Iterator(List* pList, IteratorHandler handler);

/*********************************
*****@function  
*****@para      pList           
*****@return    
**********************************/
int List_Count(List* pList);
#endif
