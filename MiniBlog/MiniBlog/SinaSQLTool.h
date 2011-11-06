#pragma once
#include "SQLiteBase.h"
#include "TaskMgr.h"

class CSinaSQLTool
{
public:
	CSinaSQLTool(CSQLiteBase *pSQLBase);
	~CSinaSQLTool(void);
	HRESULT AddFans(LPUSERINFO lpUI);
	HRESULT PrepareGetFans();
	HRESULT GetFans(LPWSTR lpUID,LPWSTR lpUser,LPWSTR lpPwd);
	HRESULT UpdateLastLogin(LPWSTR lpUID,DWORD dwStatus);
private:
	CSQLiteBase *m_pSQLBase;
};

