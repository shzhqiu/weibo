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

	DWORD GetFieldCount();
	HRESULT Reset();
	HRESULT Next();
	HRESULT GetFieldValue(LPCWSTR lpFieldName, int* pnValue);
	HRESULT GetFieldValue(int nIndex, int* pnValue);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, DWORD* pdwValue);
	HRESULT GetFieldValue(int nIndex, DWORD* pdwValue);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, LONGLONG* pllValue);
	HRESULT GetFieldValue(int nIndex, LONGLONG* pllValue);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, double* pdbValue);
	HRESULT GetFieldValue(int nIndex, double* pdbValue);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, LPWSTR lpszValue, DWORD* pdwSize);
	HRESULT GetFieldValue(int nIndex, LPWSTR lpszValue, DWORD* pdwSize);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, LPFILETIME pftVal);
	HRESULT GetFieldValue(int nIndex, LPFILETIME pftVal);
	HRESULT GetFieldValue(LPCWSTR lpFieldName, void* pData,DWORD* pdwSize);
	HRESULT GetFieldValue(int nIndex, void* pData,DWORD* pdwSize);
	BOOL    IsEof();

private:
	HRESULT InitDB();
	HRESULT CheckVersion();
	HRESULT RollbackTrans();
	sqlite3 * GetSQdb(){return m_pSQLiteDB;};
	sqlite3_stmt*	GetSQstmt(){return m_pSTMT;};
	void    Close();

	int GetFieldIndexByName(LPCWSTR lpszFieldName);
	inline BOOL CheckFieldIndex(int nIndex);
	HRESULT	SetDBVersion();

	BOOL CheckAdjustBindFieldIndex(int& nIndex);
private:
	sqlite3	*		m_pSQLiteDB; 
	sqlite3_stmt*	m_pSTMT; 
	BOOL            m_bEof;
};

