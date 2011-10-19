#pragma once
#include "./sqlite/inc/sqlite3.h"
class CSQLiteBase
{
public:
	CSQLiteBase(void);
	~CSQLiteBase(void);
public:
	HRESULT OpenDB();
	HRESULT ExecuteSQL(LPCWSTR lpszSQL);
	HRESULT BeginTrans();
	HRESULT CommitTrans();



	HRESULT PrepareSQL(LPCWSTR lpszSQL);
	HRESULT BindFieldValue(int nIndex, int nValue);
	HRESULT BindFieldValue(int nIndex, DWORD dwValue);
	HRESULT BindFieldValue(int nIndex, LONGLONG llValue);
	HRESULT BindFieldValue(int nIndex, double dbValue);
	HRESULT BindFieldValue(int nIndex, LPCWSTR lpszValue);
	HRESULT BindFieldValue(int nIndex, LPFILETIME pftVal);
	HRESULT BindFieldValue(int nIndex, void* pData,DWORD dwSize);
	HRESULT ExecuteSQL();
	//HRESULT OpenRecordSet(IRctSQRecordSet** ppRecordSet);
private:
	HRESULT InitDB();
	HRESULT CheckVersion();
	HRESULT RollbackTrans();
	sqlite3 * GetSQdb(){return m_pSQLiteDB;};
	sqlite3_stmt*	GetSQstmt(){return m_pSTMT;};
	HRESULT Next();
	void    Close();

	BOOL CheckAdjustBindFieldIndex(int& nIndex);
private:
	sqlite3	*		m_pSQLiteDB; 
	sqlite3_stmt*	m_pSTMT; 
	BOOL            m_bEof;
};

