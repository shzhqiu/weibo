#include "StdAfx.h"
#include "ADTask.h"
#include <shlwapi.h>
#include <winhttp.h>
#pragma comment (lib,"winhttp.lib")

#define SECOND(x) (x*1000)
#define MINUTE(x) (x*(60*1000))
BOOL GetHttpResponse(PBYTE pBufOut,DWORD& dwBuf,HINTERNET hRequest)
{
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
}


CADTask::CADTask(void)
{
}


CADTask::~CADTask(void)
{
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
	if (!lpTaskParam || lpTaskParam->dwTaskType != ACT_CLICK_AD)
	{
		return E_FAIL;
	}
	return ProcessTask(lpTaskParam);
}
HRESULT CADTask::ProcessTask(LPTASK_PARAM lpTaskParam)
{
	BOOL bResults=FALSE;
	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("login.sina.com.cn");
	TCHAR szParam[1024]  = _T("sso/login.php");
	TCHAR szHeader[1024] = {0};
	char szPost[1024]   = {0};
	int nPort = 80;
	MyParseURL(lpTaskParam->ad.szURL,szServer,szParam,nPort);
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