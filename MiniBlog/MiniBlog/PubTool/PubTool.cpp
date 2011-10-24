#include "StdAfx.h"
#include "PubTool.h"
#include  "afxtempl.h " 
#include  "nb30.h "
#include "md5wrapper.h"

#pragma comment(lib,"netapi32.lib")

#include <winhttp.h>
#pragma comment (lib,"winhttp.lib")

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
		int nPos1 = strURL.Find(_T("/"));
		nPort = 80;
		strS = strURL.Left(nPos1);
		_stprintf(pServer,_T("%s"),strS.GetBuffer());
		CString strObj = strURL.Right(strURL.GetLength() - nPos1-1);
		_stprintf(pObject,_T("%s"),strObj.GetBuffer());
		
	}
	else
	{

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
	}


	return TRUE;
}
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


int GetMAC(LPTSTR  pMac,int index/* = 0*/)
{
   
		NCB ncb;     
		typedef struct _ASTAT_     
		{
			ADAPTER_STATUS   adapt;  
			NAME_BUFFER   NameBuff   [30];     
		}ASTAT,   *   PASTAT;     
		ASTAT Adapter;     

		typedef struct _LANA_ENUM     
		{   //   le 
			UCHAR   length;  
			UCHAR   lana[MAX_LANA];     
		}LANA_ENUM   ;     
		LANA_ENUM lana_enum;     

		UCHAR uRetCode;     
		memset(&ncb, 0, sizeof(ncb));     
		memset(&lana_enum, 0, sizeof(lana_enum));     

		ncb.ncb_command = NCBENUM;     
		ncb.ncb_buffer = (unsigned char *)&lana_enum;     
		ncb.ncb_length = sizeof(LANA_ENUM);  
		ncb.ncb_lana_num = index;
		uRetCode = Netbios(&ncb);     
		if(uRetCode != NRC_GOODRET)     
			return uRetCode;     

		for(int lana=0; lana<lana_enum.length; lana++)     
		{
			ncb.ncb_command = NCBRESET; 
			ncb.ncb_lana_num = lana_enum.lana[lana]; 
			uRetCode = Netbios(&ncb);   
			if(uRetCode == NRC_GOODRET)  
				break; 
		} 
		if(uRetCode != NRC_GOODRET)
			return uRetCode;     

		memset(&ncb, 0, sizeof(ncb));  
		ncb.ncb_command = NCBASTAT;  
		ncb.ncb_lana_num = lana_enum.lana[0];
		strcpy((char*)ncb.ncb_callname, "*"); 
		ncb.ncb_buffer = (unsigned char *)&Adapter;
		ncb.ncb_length = sizeof(Adapter);
		uRetCode = Netbios(&ncb);  
		if(uRetCode != NRC_GOODRET)   
			return uRetCode;     
		_stprintf(pMac,_T("%02X-%02X-%02X-%02X-%02X-%02X"),     
			Adapter.adapt.adapter_address[0],     
			Adapter.adapt.adapter_address[1],     
			Adapter.adapt.adapter_address[2],     
			Adapter.adapt.adapter_address[3],     
			Adapter.adapt.adapter_address[4],     
			Adapter.adapt.adapter_address[5]  
		); 
		return 0;   


}
char *GetMD5(char * p)
{
	char *pRetString = NULL;
	md5wrapper md5;
	pRetString = md5.getHashFromString(p);
	return pRetString;
}
BOOL CheckForUpdate()
{
	PBYTE pBuf = HttpGet(SERVER_UPDATE_URL);
	float fVer = atof((char*)pBuf);
	if (fVer > CURRENT_VERSION)
	{
		return TRUE;
	}
	else
		return FALSE;
}
PBYTE HttpGet(LPTSTR lpURL)
{
	PBYTE pBuf = new BYTE[1024*1024];
	BOOL bResults = FALSE;
	HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("www.centmind.com");
	TCHAR szParam[1024]  = {0};
	TCHAR szHeader[1024] = {0};
	char szPost[1024]   = {0};
	int nPort = 80;

	MyParseURL(lpURL,szServer,szParam,nPort);

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

	
	return pBuf;

}
void StartUpdate(LPCTSTR lpUpdateEXE)
{

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	CHAR temp = _T('\"');
	CString strCmdLine;
	strCmdLine = CString(temp) + lpUpdateEXE + CString(temp);
	CHAR* p_CmdLine;
	//p_CmdLine=strCmdLine.GetBuffer(strCmdLine.GetLength());
	if( !CreateProcess( NULL, // No module name (use command line). 
		strCmdLine.GetBuffer(),		  // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		//AfxMessageBox( "CreateProcess failed." );
		return;
	}
	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}
CString GetModuleDirectory(HMODULE hModule)
{
	CString strFilePath;
	GetModuleFileName(hModule,strFilePath.GetBuffer(MAX_PATH),MAX_PATH);
	strFilePath.ReleaseBuffer();
	int iStart = strFilePath.ReverseFind('\\');
	strFilePath = strFilePath.Mid(0,iStart + 1);
	return strFilePath;	
}