#include "StdAfx.h"
#include "ADTask.h"
#include <shlwapi.h>
#include <winhttp.h>
#pragma comment (lib,"winhttp.lib")

#define SECOND(x) (x*1000)
#define MINUTE(x) (x*(60*1000))
static BOOL GetHttpResponse(PBYTE pBufOut,DWORD& dwBuf,HINTERNET hRequest)
{
#ifdef _DEBUG
	dwBuf = 0;
	DWORD dwSize = 0;
	BOOL bResults = WinHttpReceiveResponse( hRequest, NULL);
	if(bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			bResults = WinHttpQueryDataAvailable( hRequest, &dwSize);

			// Read the Data.
			ZeroMemory(pBufOut+dwBuf, dwSize+1);
			DWORD dwDownloaded=0;
			WinHttpReadData( hRequest, (LPVOID)(pBufOut+dwBuf), 
				dwSize, &dwDownloaded);
			dwBuf+=dwDownloaded;
		} while (dwSize > 0);
	}
	return bResults;

#else
	return TRUE;
#endif // _DEBUG

}


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
#define HTTP_HEADER _T("http://")
#define HTTP_HEADERLEN 7

BOOL MyParseURL(LPCTSTR pstrURL,LPTSTR pServer, LPTSTR pObject, int & nPort)
{
	ASSERT(pstrURL != NULL);
	if (pstrURL == NULL)
		return FALSE;
	CString strURL = pstrURL;
	CString strS;
	CString strO;
	if (strURL.Find(HTTP_HEADER,0) < 0)
	{
		return FALSE;
	}
	int nPos = strURL.Find(_T(":"),HTTP_HEADERLEN + 1);
	int nPos1 = strURL.Find(_T("/"),HTTP_HEADERLEN + 1);
	if(nPos < 0)
	{
			nPort = 80;
			strS = strURL.Mid(HTTP_HEADERLEN,nPos1-HTTP_HEADERLEN);
			_stprintf(pServer,_T("%s"),strS.GetBuffer());
	}
	else
	{
		CString strPort;
		strPort = strURL.Mid(nPos+1,nPos1-nPos-1);
		nPort = _tstoi(strPort.GetBuffer());
		strS = strURL.Mid(HTTP_HEADERLEN,nPos-HTTP_HEADERLEN);
		_stprintf(pServer,_T("%s"),strS.GetBuffer());

	}

	CString strObj = strURL.Right(strURL.GetLength() - nPos1-1);
	_stprintf(pObject,_T("%s"),strObj.GetBuffer());
	

	return TRUE;
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
}
HRESULT CADTask::ProcessTask()
{
	WaitForSingleObject(m_hEvent,INFINITE);
	CAutoLock lock(&m_Lock);
	BOOL bResults=FALSE;
	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("login.sina.com.cn");
	TCHAR szParam[1024]  = _T("sso/login.php");
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
	return S_OK;
}