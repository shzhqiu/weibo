#include "StdAfx.h"
#include "TaskMgr.h"
#include "SinaSvr.h"
#include <winhttp.h>
BOOL GetHttpResponse(PBYTE pBufOut,DWORD& dwBuf,HINTERNET hRequest);
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
		//return;
	}
	srand(GetTickCount());
	int rnd = rand();
	TCHAR szRand[1024] = {0};
	_stprintf(szRand,_T("hello test %d"),rnd);
	
	BOOL bResults=FALSE;
	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("1appbo.sinaapp.com");
	TCHAR szParam[1024]  = _T("?Id=1&sid=1");
	TCHAR szHeader[1024] = {0};
	char szPost[1024]   = {0};
	int nPort = 80;

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

/*	
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