
#include "funclist.h"
#include "sched.h"
#include "output_debug.h"
#include <injectcommon.h>
#include <assert.h>


#define  FUNC_LIST_VEC_ASSERT() \
do\
{\
	assert(this->m_FuncVecs->size() == this->m_ParamVecs->size());\
	assert(this->m_ParamVecs->size() == this->m_FuncUseVecs->size());\
}while(0)


CFuncList::CFuncList()
{
    InitializeCriticalSection(&m_FuncListCS);
	m_pFuncVecs = new std::vector<FuncCall_t>();
	m_pFuncUseVecs = new std::vector<int>();
	m_pParamVecs = new std::vector<LPVOID>();
    assert(m_pFuncVecs->size() == 0);
    assert(m_pParamVecs->size() == 0);
    assert(m_pFuncUseVecs->size() == 0);
}

int CFuncList::__RemoveFunc(FuncCall_t pFunc)
{
    int ret = -ERROR_NOT_FOUND;
    int findidx=-1;
    UINT i;
    EnterCriticalSection(&(this->m_FuncListCS));
    FUNC_LIST_VEC_ASSERT();
    for(i=0; i<this->m_pFuncVecs->size() ; i++)
    {
        if(pFunc == NULL || pFunc == this->m_pFuncVecs->at(i))
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        if(this->m_pFuncUseVecs->at(findidx) > 0)
        {
            ret = 0;
        }
        else
        {
            ret = 1;
            this->m_pFuncUseVecs->erase(this->m_pFuncUseVecs->begin() + findidx);
            this->m_pFuncVecs->erase(this->m_pFuncVecs->begin() + findidx);
            this->m_pParamVecs->erase(this->m_pParamVecs->begin() + findidx);
        }
    }
    LeaveCriticalSection(&(this->m_FuncListCS));
    if(ret < 0)
    {
        SetLastError(-ret);
    }
    return ret;
}


CFuncList::~CFuncList()
{
    int ret;
    do
    {
        ret = this->__RemoveFunc(NULL);
        if(ret == 0)
        {
            /*if we can not remove one ,just sched out*/
            SchedOut();
        }
    }
    while(ret >= 0);

    assert(this->m_pFuncUseVecs->size() == 0);
    FUNC_LIST_VEC_ASSERT();
	delete this->m_pFuncUseVecs;
	this->m_pFuncUseVecs = NULL;
	delete this->m_pFuncVecs;
	this->m_pFuncVecs = NULL;
	delete this->m_pParamVecs ;
	this->m_pParamVecs = NULL;
    DeleteCriticalSection(&(this->m_FuncListCS));
}

int CFuncList::AddFuncList(FuncCall_t pFunc,PVOID pParam)
{
    int ret = 1;
    int findidx = -1;
    UINT i;

    EnterCriticalSection(&(this->m_FuncListCS));
    FUNC_LIST_VEC_ASSERT();
    for(i=0; i<this->m_pFuncVecs->size(); i++)
    {
        if(this->m_pFuncVecs->at(i) == pFunc)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >=0)
    {
        ret = 0;
    }
    else
    {
        ret = 1;
        this->m_pFuncVecs->push_back(pFunc);
        this->m_pParamVecs->push_back(pParam);
        this->m_pFuncUseVecs->push_back(0);
    }
    LeaveCriticalSection(&(this->m_FuncListCS));
    return ret;
}


int CFuncList::RemoveFuncList(FuncCall_t pFunc)
{
    int ret=0;

    do
    {
        ret = this->__RemoveFunc(pFunc);
        if(ret == 0)
        {
            SchedOut();
        }
    }
    while(ret == 0);

    return ret > 0 ? 1 : 0;
}

FuncCall_t CFuncList::__GetFuncCall(int idx,LPVOID& pParam)
{
    FuncCall_t pFunc = NULL;
	int tmpint;

    EnterCriticalSection(&(this->m_FuncListCS));
    if(this->m_pFuncVecs->size() > (UINT)idx)
    {
        pFunc = this->m_pFuncVecs->at(idx);
		tmpint = this->m_pFuncUseVecs->at(idx);
		tmpint ++;
        this->m_pFuncUseVecs->at(idx) = tmpint;
    }
    LeaveCriticalSection(&(this->m_FuncListCS));
    return pFunc;
}

int CFuncList::__PutFuncCall(FuncCall_t pFunc)
{
    int idx=-1;
    UINT i;
    EnterCriticalSection(&(this->m_FuncListCS));
    for(i=0; i<this->m_pFuncVecs->size(); i++)
    {
        if(this->m_pFuncVecs->at(i) == pFunc)
        {
            idx = i+1;
            break;
        }
    }
    LeaveCriticalSection(&(this->m_FuncListCS));
    return idx;
}

int CFuncList::CallList(LPVOID pParam)
{
    FuncCall_t pFunc=NULL;
    LPVOID pCallParam=NULL;
    int totalret=0;
    int ret;
    int idx;

    idx = 0;
    while(1)
    {
        pFunc = this->__GetFuncCall(idx,pCallParam);
        if(pFunc == NULL)
        {
            break;
        }

        ret = pFunc(pCallParam,pParam);
        if(ret < 0)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("Call<0x%p> Function Error(%d)\n",pFunc,ret);
            totalret = ret;
        }

        idx = this->__PutFuncCall(pFunc);
    }

    return totalret;
}

