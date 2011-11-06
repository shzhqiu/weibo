#include "StdAfx.h"
#include "PubTool.h"
#include  "afxtempl.h " 
#include  "nb30.h "
#include "md5wrapper.h"
#include "Des.h"
#include "base64.h"
#include "../CommonDef.h"

#pragma comment(lib,"netapi32.lib")

#include <winhttp.h>
#pragma comment (lib,"winhttp.lib")

#define HTTP_HEADER _T("http://")
#define HTTP_HEADERLEN 7

static char g_key[]={'%','r','4','H','J','9','o','0'};
TCHAR g_szClientID[33] = {0};
TCHAR g_szUpdateURL[256] = {0};


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

#pragma comment (lib, "Version.lib")   

BOOL GetMyProcessVer(CString& strver)   //用来取得自己的版本号   
{   
	TCHAR strfile[MAX_PATH];   
	GetModuleFileName(NULL, strfile, sizeof(strfile));  //这里取得自己的文件名   

	DWORD dwVersize = 0;   
	DWORD dwHandle = 0;   

	dwVersize = GetFileVersionInfoSize(strfile, &dwHandle);   
	if (dwVersize == 0)   
	{   
		return FALSE;   
	}   

	TCHAR szVerBuf[8192] = _T("");   
	if (GetFileVersionInfo(strfile, 0, dwVersize, szVerBuf))   
	{   
		VS_FIXEDFILEINFO* pInfo;   
		UINT nInfoLen;   

		if (VerQueryValue(szVerBuf, _T("\\"), (LPVOID*)&pInfo, &nInfoLen))  
		{  
			/*
			dwms1 = HIWORD(pInfo->dwFileVersionMS);
			dwms2 = LOWORD(pInfo->dwFileVersionMS);
			dwls1 = HIWORD(pInfo->dwFileVersionLS);
			dwls2 = LOWORD(pInfo->dwFileVersionLS);
			*/
			strver.Format(_T("%d.%d.%d.%d"), HIWORD(pInfo->dwFileVersionMS),    
				LOWORD(pInfo->dwFileVersionMS), HIWORD(pInfo->dwFileVersionLS),   
				LOWORD(pInfo->dwFileVersionLS));   

			return TRUE;   
		}   
	}   
	return FALSE;   
}  
BOOL GetFileVersion(LPCTSTR lpVer, LONGLONG& lVer)
{
	DWORD dwVer[4]={0};

	if (!lpVer)
		return FALSE;

	// 解析字条串.
	_stscanf(lpVer, _T("%d.%d.%d.%d"), &dwVer[0], &dwVer[1], &dwVer[2], &dwVer[3]);
	DWORD dwMver = MAKELONG(dwVer[1], dwVer[0]);
	DWORD dwSver = MAKELONG(dwVer[3], dwVer[2]);
	lVer = dwMver;
	lVer <<= 32;
	lVer |= dwSver;

	return TRUE;
}

int CompareVersion(LPCTSTR szFile1, LPCTSTR szFile2)
{
	LONGLONG lVer1, lVer2;
	LONGLONG nRet = -1;
	// 获取两个文件的版本.
	if (!GetFileVersion(szFile1, lVer1))
		return nRet;
	if (!GetFileVersion(szFile2, lVer2))
		return nRet;		

	// 计算.
	nRet = (lVer1 - lVer2);

	if (nRet > 0)
	{
		return 1;
	}
	if (nRet < 0)
	{
		return -1;
	}

	return 0;
}

BOOL GetOnlineInfo(LPTSTR lpVer,LPTSTR lpURL,LPTSTR lpNote)
{
	if (!lpVer || !lpURL || ! lpNote)
	{
		return FALSE;
	}
	PBYTE pBuf = HttpGet(SERVER_UPDATE_URL,TRUE);
	int nLen = 0;
	if(!pBuf )
		return FALSE;
	nLen = _tcslen((LPCWSTR)pBuf);
	if (nLen >1024)
		return FALSE;

	TCHAR szBuf[1024] = {0};
	CM_Decrypt(szBuf,pBuf);
	nLen = _tcslen(szBuf);
	delete [] pBuf;

	TCHAR *ch = NULL;
	TCHAR *ch1 = szBuf;

	ch = _tcsstr(szBuf,_T(","));
	_tcsncpy(lpVer,szBuf+1,ch - szBuf -1);	
	ch1 = _tcsstr(ch+1,_T(","));
	_tcsncpy(lpURL,ch+1,ch1 - ch -1);
	_tcsncpy(lpNote,ch1+1,nLen - 2 - (ch1 - szBuf));

	return TRUE;

}

BOOL CheckForUpdate()
{
	
	TCHAR szLastVer[16] = {0};
	TCHAR szNote[1024] = {0};
	GetOnlineInfo(szLastVer,g_szUpdateURL,szNote);

	CString strCurVer;
	GetMyProcessVer(strCurVer);

	int nRet = CompareVersion(szLastVer,strCurVer.GetBuffer());
	
	return (nRet > 0);
}

PBYTE HttpGet(LPTSTR lpURL,BOOL bNeedRet)
{
	PBYTE pBuf = NULL;
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

	pBuf = new BYTE[1024*1024];
	ZeroMemory(pBuf,1024*1024);
	DWORD dwLen;
	GetHttpResponse(pBuf,dwLen,hRequest);
	if (!bNeedRet)
	{
		delete[] pBuf;
		pBuf = NULL;
	}

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

	if( !CreateProcess( lpUpdateEXE, // No module name (use command line). 
		g_szUpdateURL,		  // Command line. 
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
#ifdef _DEBUG
		AfxMessageBox( _T("CreateProcess failed." ));
#endif // _DEBUG
		return;
	}
	// Wait until child process exits.
//	WaitForSingleObject( pi.hProcess, INFINITE);
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

void	hex2bin(char * bin,	const char *val)
{
	int tmp=*((const int*)val);
	//cout << "tmp = " << tmp << "\n";
	int bit_number = strlen(val);
	char buf[3];
	for (int i = 0; i < bit_number/2 ;i++)
	{
		buf[0] = val[i*2];
		buf[1] = val[2*i+1];
		buf[2] = '\0';

		bin[i] = strtoul(buf,NULL,16);
	}
}

static LPSTR StringToHex(LPSTR lpHex,PBYTE lpSrc,int len)
{

    unsigned char* pDes = (unsigned char*)lpHex;
	unsigned char* pSrc = (unsigned char*)lpSrc;
    char  buf[2];

    long dwSize = len;
    for (long dwIndex = 0; dwIndex < dwSize; ++dwIndex)
    {
        unsigned char c0 = *pSrc >> 4;  
        if ( c0 >= 0x0 && c0 <= 0x9)
        {
            buf[0] = c0 - 0 + '0';
        }
        else 
        {
            buf[0] = c0 - 10 + 'A';
        }
        unsigned char c1 = *pSrc++ & 0x0F;
        if ( c1 >= 0x0 && c1 <= 0x9)
        {
            buf[1] = c1 - 0 + '0';
        }
        else 
        {
            buf[1] = c1 - 10 + 'A';
        }
        *(pDes++) = buf[0];
        *(pDes++) = buf[1];
    }
    return lpHex;
}

void CM_Encrypt(LPWSTR lpOut,LPCWSTR lpIn)
{
	char szIn[MAX_PATH] = {0};
	char des[MAX_PATH*2] = {0};
	char hex[MAX_PATH*4] = {0};
	WideCharToMultiByte(CP_ACP,0,lpIn,-1,szIn,MAX_PATH,NULL,NULL);
	Des_Go(des, szIn, strlen(szIn), g_key, sizeof(g_key), DES_ENCRYPT);
	int len = ((strlen(szIn)+7)/8)*8;
	StringToHex(hex,(PBYTE)des,len);
	MultiByteToWideChar(CP_ACP,0,hex,-1,lpOut,MAX_PATH*2);
}

void CM_Decrypt(LPWSTR lpOut,PBYTE lpIn)
{
	//char szBase64[MAX_PATH] = {0};
	char szBuff[MAX_PATH*2]   = {0};
	char szOut[MAX_PATH]    = {0};
	//WideCharToMultiByte(CP_ACP,0,lpIn,-1,szBase64,MAX_PATH,NULL,NULL);
	hex2bin(szBuff,(char*)lpIn);
	Des_Go(szOut, szBuff, strlen(szBuff), g_key, sizeof(g_key), DES_DECRYPT);
	MultiByteToWideChar(CP_ACP,0,szOut,-1,lpOut,MAX_PATH);

}


void InitClientID()
{
	TCHAR szMAC[18];
	GetMAC(szMAC);
	TCHAR szMagic[100] = {0};
	_stprintf(szMagic,_T("www.centmind.com/www.weibojuntuan.com/%s"),szMAC);

	char cMagic[100] = {0};
	WideCharToMultiByte(CP_ACP,0,szMagic,-1,cMagic,100,NULL,NULL);
	char *cHash = GetMD5(cMagic);
	MultiByteToWideChar(CP_ACP,0,cHash,-1,g_szClientID,33);
	delete [] cHash;
}
LPCTSTR GetClientID(LPTSTR lpCID)
{
	if (lpCID)
		_tcscpy(lpCID,g_szClientID);
	return lpCID;
}