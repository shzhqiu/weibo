#pragma once
#include "SQLiteBase.h"

class CSinaSQLTool
{
public:
	CSinaSQLTool(CSQLiteBase *pSQLBase);
	~CSinaSQLTool(void);
	HRESULT AddFans(LPCWSTR lpUID,LPCWSTR lpUser,LPCWSTR pwd);
	HRESULT PrepareGetFans();
	HRESULT GetFans(LPWSTR lpUID,LPWSTR lpUser,LPWSTR lpPwd);

private:
	CSQLiteBase *m_pSQLBase;
};

