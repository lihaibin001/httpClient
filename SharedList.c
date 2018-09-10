#include "SharedList.h"
#include "CommonDefine.h"
#include <stdlib.h>
#include <string.h>
#define OP_FREE 0
#define OP_WRITE 1
#define OP_READ 2
#define OP_ALL 3

#define RET_ABANDON (RET_USER_DEFINE - 1)

SharedList* SharedList_Create(int nItemLen)
{
	List* pList = List_Create(nItemLen);
	if (pList != NULL)
	{
		SharedList* pSharedList = (SharedList*)TUCMalloc(sizeof(SharedList));

		if (pSharedList != NULL)
		{
			memset(pSharedList, 0, sizeof(SharedList));
			pSharedList->op_code = OP_FREE;
			pSharedList->pList = pList;
			return pSharedList;
		}
		else
		{
			free(pList);
			pList = NULL;
		}
	}
	return NULL;
}

int SharedList_Destroy(SharedList* pList)
{
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	free(pList->pList);
	pList->pList = NULL;
	free(pList);
	pList = NULL;
	return RET_FUNCTION_OK;
}

int SharedList_Add(SharedList* pList, void* pItem)
{
	int res = RET_FUNCTION_OK;
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	res = SharedList_Lock(pList);
	if (res == RET_FUNCTION_OK)
	{
		res = List_Add(pList->pList, pItem);
		SharedList_Unlock(pList);
	}
	return res;
}

void* SharedList_FindCopy(SharedList* pList, int nIndex)
{
	if (pList == NULL)
	{
		return NULL;
	}
	if (SharedList_Lock(pList) == RET_FUNCTION_OK)
	{
		void* pData = List_FindCopy(pList->pList, nIndex);
		SharedList_Unlock(pList);
		return pData;
	}
	return NULL;
}

int SharedList_Remove(SharedList* pList, int nIndex)
{
	int res = RET_FUNCTION_OK;
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	res = SharedList_Lock(pList);
	if (res == RET_FUNCTION_OK)
	{
		res = List_Remove(pList->pList, nIndex);
		SharedList_Unlock(pList);
	}
	return res;
}

int ShareList_update(ShareList *pList, int nIndex, void *pItem)
{
	int res;
	res = SharedList_Lock(pList);
	if (res == RET_FUNCTION_OK)
	{
		res = List_Update(pList, nIndex, pItem);
		SharedList_Unlock(pList);
	}
	return res;	
}

int SharedList_Iterator(SharedList* pList, IteratorHandler handler)
{
	int res = RET_FUNCTION_OK;
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	res = SharedList_Lock(pList);
	if (res == RET_FUNCTION_OK)
	{
		res = List_Iterator(pList->pList, handler);
		SharedList_Unlock(pList);
	}
	return res;
}

int SharedList_Count(SharedList* pList)
{
	if (pList == NULL)
	{
		return 0;
	}
	else
	{
		if (SharedList_Lock(pList) == RET_FUNCTION_OK)
		{
			int res = List_Count(pList->pList);
			SharedList_Unlock(pList);
			return res;
		}
		else
		{
			return 0;
		}
	}
}

int SharedList_Lock(SharedList* pList)
{
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	if (pList->op_code != OP_FREE)
	{
		return RET_ABANDON;
	}
	pList->op_code = OP_ALL;
	return RET_FUNCTION_OK;
}

int SharedList_Unlock(SharedList* pList)
{
	if (pList == NULL)
	{
		return RET_BAD_PARA;
	}
	pList->op_code = OP_FREE;
	return RET_FUNCTION_OK;
}

