#include "StdAfx.h"
#include "TaskMgr.h"
#include "SinaSvr.h"

CTaskMgr::CTaskMgr(void)
{
	m_dwLastTick = GetTickCount();
	m_pSvr = NULL;
}


CTaskMgr::~CTaskMgr(void)
{
}
void CTaskMgr::GetTask()
{
	if (!CheckElapsed())
	{
		return;
	}
	srand(GetTickCount());
	int rnd = rand();
	//m_pDB->AddFans("name","pwd");
/*	TCHAR szPost[1024] = {0};
	_stprintf(szPost,_T("hello test %d"),rnd);
	ZeroMemory(&m_taskParam,sizeof(m_taskParam));
	m_taskParam.dwTaskType = ACT_POST_SINA;
	_tcscpy(m_taskParam.post.szContent,szPost);
	m_pSvr->AddTask(&m_taskParam);
	*/
}

BOOL CTaskMgr::CheckElapsed()
{	
	BOOL bRet = FALSE;
	do 
	{
		if((GetTickCount() - m_dwLastTick) < 1000*60)
			break;
		bRet = TRUE;
	
	} while (FALSE);
	m_dwLastTick = GetTickCount();
	return bRet;

}