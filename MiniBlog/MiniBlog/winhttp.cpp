// winhttp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ATS_Helper.h"

#include <winhttp.h>
#pragma comment (lib,"winhttp.lib")


//////////////////////////////////////////////////////////////////////////
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
void urlenc(LPCTSTR strInput,LPTSTR strOutput)   
{   
	_tcscpy(strOutput,strInput);
	//	return;
	DWORD dwNum = 0;
	dwNum = WideCharToMultiByte(CP_UTF8,NULL,strInput,-1,NULL,0,NULL,FALSE);//计算要使用空间
	CHAR* pBuffAnsi = new CHAR[dwNum+1];
	WideCharToMultiByte (CP_UTF8,NULL,strInput,-1,(PCHAR)pBuffAnsi,dwNum,NULL,FALSE);
	CHAR bufOut[MAX_PATH];
	char*   buf = bufOut;
	const   char*   str = pBuffAnsi;
	const   char   special[]   =   "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}~%";   /*   special   characters   */   
	char   tmp[10];   /*   temp   buffer   */   
	buf[0]   =   '\0';   
	for(size_t   i=0;   i< strlen(str);   i++)   
	{   
		char   c   =   str[i];   
		if(strchr(special,   c))   /*   special   character   */   
		{   
			sprintf(tmp,   "%%%02X",   (unsigned   char)c);   
			strcat(buf,   tmp);   
		}   
		else   if(c   ==   ' ')   /*   blank   */   
			strcat(buf,   "+");   
		else   
		{   
			if(c   <   0)   /*   none   ASCII   character   */   
			{   
				sprintf(tmp,   "%%%02X",   (unsigned   char)str[i]/*,   (unsigned   char)str[i+1]*/);   
				strcat(buf,   tmp);   
				//	++i;   
			}   
			else   /*   ASCII   character   */   
			{   
				sprintf(tmp,   "%c",   c);   
				strcat(buf,   tmp);   
			}   
		}   
	}   
	
	dwNum = MultiByteToWideChar(CP_UTF8,NULL,bufOut,-1,NULL,0);//计算要使用空间
	MultiByteToWideChar (CP_UTF8,NULL,bufOut,-1,strOutput,dwNum);	
}
void WriteResponseInfo(PBYTE pResponse,LPCTSTR lpoutfile)
{
	if (pResponse && lpoutfile)
	{
		FILE *f = _tfopen(lpoutfile,_T("wb"));
		if(f)
		{
			
			fwrite(pResponse,1,strlen((char*)pResponse),f);
			fclose(f);
			f = NULL;
		}
	}
	
}
int main(int argc, char* argv[])
{
	printf("Hello World!\n");

	BOOL bResults=FALSE;
	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	
	//http://login.sina.com.cn/sso/login.php
	TCHAR szServer[1024] = _T("login.sina.com.cn");
	TCHAR szParam[1024]  = _T("sso/login.php");
	TCHAR szHeader[1024] = _T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n");
	char szPost[1024]   = "username=shzhqiu@hotmail.com&password=93732717&entry=miniblog&act=1&from=referer%3Awww_index";
	PBYTE pBuf = new BYTE[1024*1024];

	hSession = WinHttpOpen(0,  
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME, 
		WINHTTP_NO_PROXY_BYPASS, 0 );
	
	if(hSession)
		WinHttpSetTimeouts( hSession, 600000, 600000, 600000, 600000);

	if( hSession )
		hConnect = WinHttpConnect( hSession,szServer,80, 0 );

	if (hConnect)
        hRequest = WinHttpOpenRequest( hConnect, L"POST", szParam, 
		NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, 
		0);
	int nLen = strlen(szPost);
	// Send a Request.
    if (hRequest) 
		bResults = WinHttpSendRequest( hRequest, szHeader,-1, szPost, nLen,nLen,0);

	DWORD dwLen;
	GetHttpResponse(pBuf,dwLen,hRequest);

	WriteResponseInfo(pBuf,_T("out.txt"));
	if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
	
	delete []pBuf;
	return 0;
}

