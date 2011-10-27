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


	hr = m_pSQLBase->PrepareSQL(_T("INSERT INTO tblSubfans (uid,name,pwd,lastlogin,status,type) VALUES(?,?,?,'2011-01-01',1,1);"));


	hr = m_pSQLBase->BindFieldValue(0,lpUID); 
	hr |= m_pSQLBase->BindFieldValue(1,lpUser); 
	hr |= m_pSQLBase->BindFieldValue(2,pwd); 
	hr |= m_pSQLBase->ExecuteSQL();

	return hr;

}
HRESULT CSinaSQLTool::PrepareGetFans()
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;


	hr = m_pSQLBase->ExecuteSQL(_T("SELECT * FROM tblSubfans;)"));
	if (SUCCEEDED(hr))
	{
		hr = m_pSQLBase->Reset();
	}
	return hr;

}

HRESULT CSinaSQLTool::GetFans(LPWSTR lpUID,LPWSTR lpUser,LPWSTR lpPwd)
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;
	if (m_pSQLBase->IsEof())
	{
		return S_FALSE;
	}
	DWORD dwSize = 1024;
	hr |= m_pSQLBase->GetFieldValue(1,lpUID,&dwSize);
	hr |= m_pSQLBase->GetFieldValue(2,lpUser,&dwSize);
	hr |= m_pSQLBase->GetFieldValue(3,lpPwd,&dwSize);
	hr |= m_pSQLBase->Next();
	return (hr == S_OK);

}