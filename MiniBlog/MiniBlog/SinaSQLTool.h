#pragma once
#include "SQLiteBase.h"

class CSinaSQLTool
{
public:
	CSinaSQLTool(CSQLiteBase *pSQLBase);
	~CSinaSQLTool(void);
	HRESULT AddFans(LPCWSTR lpUID,LPCWSTR lpUser,LPCWSTR pwd);

private:
	CSQLiteBase *m_pSQLBase;
};

