#include "StdAfx.h"
#include "SinaSQLTool.h"


CSinaSQLTool::CSinaSQLTool(CSQLiteBase *pSQLBase):
m_pSQLBase(pSQLBase)
{
}


CSinaSQLTool::~CSinaSQLTool(void)
{
}
HRESULT CSinaSQLTool::AddFans(LPUSERINFO lpUI)
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;

	// check if exist

	hr = m_pSQLBase->ExecuteSQL(_T("SELECT * FROM tblSubfans where uid = ?;)"));
	if (SUCCEEDED(hr))
	{
		hr = m_pSQLBase->Reset();
	}
	if (!m_pSQLBase->IsEof())
	{
		hr = UpdateLastLogin(lpUI->szUID,lpUI->dwStatus);
		return hr;
	}


	// insert
	hr = m_pSQLBase->PrepareSQL(_T("INSERT INTO tblSubfans (uid,name,pwd,lastlogin,status,type) VALUES(?,?,?,datetime(),?,1);"));


	hr = m_pSQLBase->BindFieldValue(0,lpUI->szUID); 
	hr |= m_pSQLBase->BindFieldValue(1,lpUI->szName); 
	hr |= m_pSQLBase->BindFieldValue(2,lpUI->szPWD); 
	hr |= m_pSQLBase->BindFieldValue(3,lpUI->dwStatus); 
	hr |= m_pSQLBase->ExecuteSQL();

	return hr;

}
HRESULT CSinaSQLTool::PrepareGetFans()
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;


	hr = m_pSQLBase->ExecuteSQL(_T("SELECT * FROM tblSubfans order by lastlogin;)"));
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
HRESULT CSinaSQLTool::UpdateLastLogin(LPWSTR lpUID,DWORD dwStatus)
{
	HRESULT hr = E_FAIL;
	if (!m_pSQLBase)
		return hr;


	hr = m_pSQLBase->PrepareSQL(_T("update tblSubfans set lastlogin = datetime(),status = ? where uid= ?;"));


	hr = m_pSQLBase->BindFieldValue(0,dwStatus); 
	hr = m_pSQLBase->BindFieldValue(1,lpUID); 
	hr |= m_pSQLBase->ExecuteSQL();

	return hr;

}