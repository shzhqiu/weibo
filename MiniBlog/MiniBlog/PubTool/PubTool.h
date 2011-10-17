#pragma once
#include "md5wrapper.h"
class CPubTool
{
public:
	CPubTool(void);
	~CPubTool(void);
	static int GetMAC(LPTSTR  pMac,int index = 0);
	static char* GetMD5(char* pSrc);
};

