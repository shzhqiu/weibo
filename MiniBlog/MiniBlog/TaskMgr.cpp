#include "StdAfx.h"
#include "TaskMgr.h"
#include "SinaSvr.h"
#include <winhttp.h>
#include "PubTool/PubTool.h"

unsigned long StrToHex(char* str)
{
	unsigned long hex,val;
	int n;

	hex=0;
	for(n=0;n<strlen(str);n++) {
		if(str[n]>='0' && str[n]<='9')
			val=(unsigned long)(str[n]-'0');
		else if(str[n]>='a' && str[n]<='f')
			val=((unsigned long)(str[n]-'a'))+0xA;
		else if(str[n]>='A' && str[n]<='F')
			val=((unsigned long)(str[n]-'A'))+0xA;
		else
			val=0;
		hex<<=4;
		hex|=val;
	}
	return hex;
}
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
	//char key[]={'c','e','n','t','m','i','n','d'};
	//%r4HJ9o0

	TCHAR szParam[1024]  = {0};
	TCHAR szID[MAX_PATH] = {0};
	TCHAR szSID[MAX_PATH] = {0};
	CM_Encrypt(szID,_T("1"));
	CM_Encrypt(szSID,_T("2"));
	_stprintf(szParam,_T("?actid=%s&sid=%s"),szID,szSID);
	/*
		char temp[256] = {0};
	char id[255],sid[256];
	memset(id, 0, sizeof(id));
	strcpy(temp, "1");Des_Go(temp, temp, strlen(temp), key, sizeof(key), DES_ENCRYPT);
	av_base64_encode(id,256,(const uint8_t *)temp,strlen(temp));
	memset(sid, 0, sizeof(sid));
	memset(temp, 0, strlen(temp));
	strcpy(temp, "2");
	Des_Go(temp, temp, strlen(temp), key, sizeof(key), DES_ENCRYPT);
	av_base64_encode(sid,256,(const uint8_t *)temp,strlen(temp));

	char param_a[1024] = {0};
	sprintf(param_a,"?Id=%s&sid=%s",id,sid);


	//TCHAR szParam[1024]  = _T("?Id=1&sid=2");
	
	int n = MultiByteToWideChar(CP_ACP,0,param_a,-1,szParam,1024);
	*/

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
#if 0


	char szBuff[1024] = {0};
	char szBuff1[1024] = {0};
	char szBuff2[1024] = {0};
	WCHAR szzzz[] = _T("5kNaO/2twnXPNbwhbnmKcQ==");
	WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pBuf,-1,szBuff,1024,NULL,NULL);
	av_base64_decode((uint8_t * )szBuff1,(char*)pBuf,1024);
	Des_Go(szBuff2, szBuff1, strlen(szBuff1), key, sizeof(key), DES_DECRYPT);
#endif
	TCHAR szOutPut[MAX_PATH] = {0};
	CM_Decrypt(szOutPut,(LPCWSTR)pBuf);
	//Des_Go(szBuff2,szBuff1,)

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