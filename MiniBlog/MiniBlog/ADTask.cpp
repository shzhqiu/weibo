#include "StdAfx.h"
#include "ADTask.h"
#include <shlwapi.h>


#define SECOND(x) (x*1000)
#define MINUTE(x) (x*(60*1000))



CADTask::CADTask(void)
{
	DWORD dwThreadId = 0;
	m_hThread = CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE)&CADTask::ThreadProc,(LPVOID) this,	0, 	&dwThreadId );
	m_hEvent = CreateEvent(NULL,FALSE,FALSE,_T("WEIBODATA_AD_EVENT"));
	//ResetEvent(m_hEvent);
	m_bClose = FALSE;
	
}


CADTask::~CADTask(void)
{
	SetEvent(m_hEvent);
	m_bClose = TRUE;
	Sleep(50);
	CloseHandle(m_hThread);
	CloseHandle(m_hEvent);
}


HRESULT CADTask::AddTask(LPTASK_PARAM lpTaskParam)
{
	CAutoLock lock(&m_Lock);

	if (!lpTaskParam || lpTaskParam->dwTaskType != ACT_CLICK_AD)
	{
		return E_FAIL;
	}
	memcpy((PVOID)&m_taskParam,(PVOID)lpTaskParam,sizeof(m_taskParam));
	SetEvent(m_hEvent);
		
	return S_OK;
	//return ProcessTask(lpTaskParam);
}
DWORD CADTask::ThreadProc (LPVOID lpRef )
{
	CADTask *pThis = (CADTask*)lpRef;
	if(!pThis)
		return E_FAIL;
	
	while(TRUE)
	{
		//CAutoLock lock(&pThis->m_Lock);
		pThis->ProcessTask();

		if (pThis->GetThreadStatus())
		{
			break;
		}
		
	}
	return S_OK;
}
HRESULT CADTask::ProcessTask()
{
	WaitForSingleObject(m_hEvent,INFINITE);
	CAutoLock lock(&m_Lock);
	BOOL bResults=FALSE;
#if 0


	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("www.centmind.com");
	TCHAR szParam[1024]  = {0};
	TCHAR szHeader[1024] = {0};
	char szPost[1024]   = {0};
	int nPort = 80;
	MyParseURL(m_taskParam.ad.szURL,szServer,szParam,nPort);
	PBYTE pBuf = new BYTE[1024*1024];

	hSession = WinHttpOpen(0,  
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME, 
		WINHTTP_NO_PROXY_BYPASS, 0 );
	
	if(hSession)
		WinHttpSetTimeouts( hSession, 30000, 60000, 30000, 30000);

	if( hSession )
		hConnect = WinHttpConnect( hSession,szServer,nPort, 0 );

	if (hConnect)
        hRequest = WinHttpOpenRequest( hConnect, L"GET", szParam, 
		NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, 
		0);
	int nLen = strlen(szPost);
	// Send a Request.
    if (hRequest) 
		bResults = WinHttpSendRequest( hRequest, szHeader,-1, szPost, nLen,nLen,0);

	DWORD dwLen;
	GetHttpResponse(pBuf,dwLen,hRequest);

	//WriteResponseInfo(pBuf,_T("out.txt"));
	if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
	
	delete []pBuf;
#endif
	return S_OK;
}