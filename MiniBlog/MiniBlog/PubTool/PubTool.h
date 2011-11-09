#pragma once
#define SafeDelete(p) {if(p) delete p;p = NULL;}

#include <math.h>

int GetMAC(LPTSTR  pMac,int index = 0);
char* GetMD5(char* pSrc);
BOOL CheckForUpdate();
PBYTE HttpGet(LPTSTR lpURL,BOOL bNeedRet = FALSE);
void StartUpdate(LPCTSTR lpUpdateEXE);
CString GetModuleDirectory(HMODULE hModule);
void CM_Encrypt(LPWSTR lpOut,LPCWSTR lpIn);
void CM_Decrypt(LPWSTR lpOut,PBYTE lpIn);
void InitClientID();
LPCTSTR GetClientID(LPTSTR lpCID);
void UTF_8ToUnicode(WCHAR* pOut,char *pText);



