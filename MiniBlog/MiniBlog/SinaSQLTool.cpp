#include "StdAfx.h"
#include "SinaSQLTool.h"


CSinaSQLTool::CSinaSQLTool(CSQLiteBase *pSQLBase):
m_pSQLBase(pSQLBase)
{
}


CSinaSQLTool::~CSinaSQLTool(void)
{
}
HRESULT CSinaSQLTool::AddFans(LPCWSTR lpUID,LPCWSTR lpUser,LPCWSTR pwd)
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;


	hr = m_pSQLBase->PrepareSQL(_T("INSERT INTO subfans (uid,name,pwd,lastlogin,status,type) VALUES(?,?,?,'2011-01-01',1,1);"));


	hr = m_pSQLBase->BindFieldValue(0,lpUID); 
	hr |= m_pSQLBase->BindFieldValue(1,lpUser); 
	hr |= m_pSQLBase->BindFieldValue(2,pwd); 
	hr |= m_pSQLBase->ExecuteSQL();

	return hr;

}
