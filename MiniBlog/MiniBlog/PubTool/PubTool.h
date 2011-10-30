#pragma once
#define SafeDelete(p) {if(p) delete p;p = NULL;}
#include "md5wrapper.h"

int GetMAC(LPTSTR  pMac,int index = 0);
char* GetMD5(char* pSrc);
BOOL CheckForUpdate();
PBYTE HttpGet(LPTSTR lpURL,BOOL bNeedRet = FALSE);
void StartUpdate(LPCTSTR lpUpdateEXE);
CString GetModuleDirectory(HMODULE hModule);
void CM_Encrypt(LPWSTR lpOut,LPCWSTR lpIn);
void CM_Decrypt(LPWSTR lpOut,PBYTE lpIn);


