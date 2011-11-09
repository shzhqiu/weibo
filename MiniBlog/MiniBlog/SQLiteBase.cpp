#include "StdAfx.h"
#include "SQLiteBase.h"

#define DB_PATH_MINIBLOG _T("client.db")
#define DB_VERSION        _T("20111006")

CSQLiteBase::CSQLiteBase(void)
{
	m_bEof = TRUE;
	m_pSTMT = NULL;
	m_pSQLiteDB = NULL;
}

CSQLiteBase::~CSQLiteBase(void)
{
	Close();
	sqlite3_close(m_pSQLiteDB); 
}


HRESULT CSQLiteBase::OpenDB()
{
	int r = sqlite3_open16(DB_PATH_MINIBLOG,&m_pSQLiteDB) ;
	if( r!= SQLITE_OK){ 
		printf("%s",sqlite3_errmsg(m_pSQLiteDB)); 
		return r;
	}
		InitDB();
	return r;
}
HRESULT CSQLiteBase::CheckVersion()
{
	return S_FALSE;
}
HRESULT CSQLiteBase::InitDB()
{
	if(!m_pSQLiteDB)
		return E_FAIL;
	static TCHAR initsql[][MAX_PATH]=
	{
		{_T("CREATE TABLE tblSubfans( ID INTEGER PRIMARY KEY, uid VCHAR(255)  unique, name VCHAR(255), pwd VCHAR(255),lastlogin DATETIME,status INTEGER,type INTEGER);")},
		{_T("CREATE TABLE tblMainID( ID INTEGER PRIMARY KEY, uid VCHAR(255)  unique,lastlogin DATETIME,type INTEGER);")},
		{_T("CREATE TABLE tblVersion(ver VCHAR(255)) unique ;")},

	};

	int nTableCnt = sizeof(initsql) / sizeof(initsql[0]);
	
	BeginTrans();
	for (int i = 0 ; i < nTableCnt; ++i)
	{
		ExecuteSQL(initsql[i]);
	}
	CommitTrans();
	SetDBVersion();
	return S_OK;

}
HRESULT CSQLiteBase::SetDBVersion()
{
	HRESULT hr = E_FAIL;
	TCHAR szSql[MAX_PATH] = {0};
	_stprintf(szSql,_T("insert into tblVersion (ver) values(%s)"),DB_VERSION);
	ExecuteSQL(szSql);
	return S_OK;

}
HRESULT CSQLiteBase::ExecuteSQL(LPCWSTR lpszSQL)
{
	HRESULT hr = E_FAIL;

	do 
	{

		hr = PrepareSQL(lpszSQL);
		if(FAILED(hr))
			break;

		hr = ExecuteSQL();
		if(FAILED(hr))
			break;
		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::PrepareSQL(LPCWSTR lpszSQL)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(lpszSQL == NULL)
			break;

		sqlite3* psqdb = GetSQdb();
		if(psqdb == NULL)
			break;

		const WCHAR *tail=NULL;
		int nRet = SQLITE_ERROR;
		while(TRUE)
		{
			Close();

			nRet = sqlite3_prepare16(psqdb, lpszSQL, -1, &m_pSTMT, (const void**)&tail);
			//如果数据库被锁住了（另一进程在进行事务处理），循环直到数据库解锁
			if(nRet == SQLITE_BUSY)
			{
				Sleep(2);
				continue;
			}
			break;
		}
		if(nRet!=SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}

void CSQLiteBase::Close()
{
	if(m_pSTMT != NULL)
	{
		sqlite3_finalize(m_pSTMT);
		m_pSTMT = NULL;
	}

}
HRESULT CSQLiteBase::Next()
{
	HRESULT hr = E_FAIL;
	sqlite3_stmt* pstmt = GetSQstmt();
	if(pstmt == NULL)
		return hr;

	int nLoopCount = 10000;
	do 
	{
		int nRet = sqlite3_step(pstmt);
		switch(nRet)
		{
		case SQLITE_ROW:
			m_bEof = FALSE;
			nLoopCount = 0;
			hr = S_OK;
			break;
		case SQLITE_DONE:
			m_bEof = TRUE;
			nLoopCount = 0;
			hr = S_FALSE;
			break;
		case SQLITE_BUSY:
		case SQLITE_LOCKED:
			//loop,until unlocked or timeout
			//			ARC_ASSERT(FALSE);
			Sleep(10);
			nLoopCount--;
			m_bEof = TRUE;
			break;
		case SQLITE_ERROR:
			nLoopCount = 0;
			m_bEof = TRUE;
			hr = E_FAIL;
			break;
		default:
			nLoopCount = 0;
			m_bEof = TRUE;
			break;
		}
	} while(nLoopCount>0);

	return hr;
}
HRESULT CSQLiteBase::ExecuteSQL()
{
	return Next();

}
HRESULT CSQLiteBase::BeginTrans()
{
	HRESULT hr = ExecuteSQL(L"begin exclusive;");
	if(FAILED(hr))
	{
	}
	return hr;
}

HRESULT CSQLiteBase::CommitTrans()
{
	HRESULT hr = ExecuteSQL(L"commit;");
	if(FAILED(hr))
	{
		RollbackTrans();
	}
	return hr;
}

HRESULT CSQLiteBase::RollbackTrans()
{
	HRESULT hr = ExecuteSQL(L"rollback;");

	return hr;
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, int nValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(m_pSTMT == NULL)
			break;
		if(!CheckAdjustBindFieldIndex(nIndex))
			break;
		if(sqlite3_bind_int(m_pSTMT,nIndex,nValue) != SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, DWORD dwValue)
{
	LONGLONG llValue = dwValue;
	return BindFieldValue(nIndex,llValue);
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, LONGLONG llValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(m_pSTMT == NULL)
			break;
		if(!CheckAdjustBindFieldIndex(nIndex))
			break;
		if(sqlite3_bind_int64(m_pSTMT,nIndex,llValue) != SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, double dbValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(m_pSTMT == NULL)
			break;
		if(!CheckAdjustBindFieldIndex(nIndex))
			break;
		if(sqlite3_bind_double(m_pSTMT,nIndex,dbValue) != SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, LPCWSTR lpszValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(m_pSTMT == NULL || lpszValue == NULL)
			break;
		if(!CheckAdjustBindFieldIndex(nIndex))
			break;
		if(sqlite3_bind_text16(m_pSTMT,nIndex,lpszValue, _tcslen(lpszValue) * sizeof(TCHAR),SQLITE_STATIC) != SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, LPFILETIME pftVal)
{
	double dValue = 0;//CRctPubTools::ConvertFromFileTime(pftVal);
	return BindFieldValue(nIndex,dValue);
}

HRESULT CSQLiteBase::BindFieldValue(int nIndex, void* pData,DWORD dwSize)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(m_pSTMT == NULL || pData == NULL || dwSize == 0)
			break;
		if(!CheckAdjustBindFieldIndex(nIndex))
			break;
		if(sqlite3_bind_blob(m_pSTMT,nIndex,pData,dwSize,SQLITE_STATIC) != SQLITE_OK)
			break;

		hr = S_OK;
	} while(FALSE);

	return hr;
}
BOOL CSQLiteBase::CheckAdjustBindFieldIndex(int& nIndex)
{
	BOOL bRet = FALSE;

	do 
	{
		if(m_pSTMT == NULL || nIndex < 0)
			break;
		nIndex++;
		if(nIndex > sqlite3_bind_parameter_count(m_pSTMT))
			break;

		bRet = TRUE;
	} while(FALSE);

	return bRet;
}


DWORD CSQLiteBase::GetFieldCount()
{
	DWORD dwCount = 0;

	do 
	{
		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		dwCount = sqlite3_column_count(pstmt);
	} while(FALSE);

	return dwCount;
}

HRESULT CSQLiteBase::Reset()
{
	HRESULT hr = E_FAIL;

	do 
	{
		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		m_bEof = TRUE;
		int nRet = sqlite3_reset(pstmt);
		if(nRet != SQLITE_OK)
			break;

		hr = Next();

	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, int* pnValue)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pnValue);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, int* pnValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(!CheckFieldIndex(nIndex) || pnValue == NULL)
			break;

		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		*pnValue = sqlite3_column_int(pstmt,nIndex);

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, DWORD* pdwValue)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pdwValue);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, DWORD* pdwValue)
{
	HRESULT hr = E_FAIL;
	if(pdwValue == NULL)
		return hr;

	LONGLONG llVal = 0;
	hr = GetFieldValue(nIndex,&llVal);
	if(SUCCEEDED(hr))
	{
		*pdwValue = (DWORD)llVal;
	}

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, LONGLONG* pllValue)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pllValue);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, LONGLONG* pllValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(!CheckFieldIndex(nIndex) || pllValue == NULL)
			break;

		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		*pllValue = sqlite3_column_int64(pstmt,nIndex);

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, double* pdbValue)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pdbValue);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, double* pdbValue)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(!CheckFieldIndex(nIndex) || pdbValue == NULL)
			break;

		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		*pdbValue = sqlite3_column_double(pstmt,nIndex);

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, LPWSTR lpszValue, DWORD* pdwSize)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,lpszValue,pdwSize);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, LPWSTR lpszValue, DWORD* pdwSize)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(!CheckFieldIndex(nIndex) || lpszValue == NULL)
			break;

		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		LPCWSTR lpszVal = (LPCWSTR)sqlite3_column_text16(pstmt,nIndex);
		if(lpszVal == NULL)
		{
			hr = E_FAIL;
			break;
		}
		DWORD dwSize = _tcslen(lpszVal) + 1;
		if(lpszVal == NULL)
		{
			*pdwSize = dwSize;
		}
		else
		{
			if(*pdwSize >= dwSize)
			{
				_tcscpy(lpszValue,lpszVal);
			}
			else
			{
				hr = E_OUTOFMEMORY;
				break;
			}
		}

		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, LPFILETIME pftVal)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pftVal);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, LPFILETIME pftVal)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(pftVal == NULL)
			break;

		double dVal = 0.0;
		hr = GetFieldValue(nIndex,&dVal);
		if(FAILED(hr))
			break;

		//CRctPubTools::ConvertToFileTime(pftVal,dVal);
		hr = S_OK;
	} while(FALSE);

	return hr;
}

HRESULT CSQLiteBase::GetFieldValue(LPCWSTR lpFieldName, void* pData,DWORD* pdwSize)
{
	if(IsEof())
		return E_FAIL;

	int nIndex = GetFieldIndexByName(lpFieldName);
	return GetFieldValue(nIndex,pData,pdwSize);
}

HRESULT CSQLiteBase::GetFieldValue(int nIndex, void* pData,DWORD* pdwSize)
{
	HRESULT hr = E_FAIL;
	do 
	{
		if(!CheckFieldIndex(nIndex) || pdwSize == NULL)
			break;

		sqlite3_stmt* pstmt = GetSQstmt();
		if(pstmt == NULL)
			break;

		int nSize = sqlite3_column_bytes16(pstmt,nIndex);
		if(pData == NULL)
		{
			*pdwSize = nSize;
		}
		else
		{
			if(*pdwSize >= nSize)
			{
				memcpy(pData,sqlite3_column_blob(pstmt,nIndex),nSize);
			}
			else
			{
				break;
			}
		}

		hr = S_OK;
	} while(FALSE);

	return hr;
}
int CSQLiteBase::GetFieldIndexByName(LPCWSTR lpszFieldName)
{
	int nIndex = -1;

	if(lpszFieldName == NULL || lpszFieldName[0] == 0)
		return nIndex;

	sqlite3_stmt* pstmt = GetSQstmt();
	if(pstmt == NULL)
		return nIndex;

	DWORD dwCount = GetFieldCount();
	for(int i=0;i<dwCount;i++)
	{
		LPCWSTR lpszName = (LPCWSTR)sqlite3_column_name16(pstmt,i);
		if(lpszName == NULL)
			continue;
		if(_tcsicmp(lpszFieldName,lpszName) == 0)
		{
			nIndex = i;
			break;
		}

	} while(FALSE);

	return nIndex;
}
inline BOOL CSQLiteBase::CheckFieldIndex(int nIndex)
{
	return (nIndex >= 0 && nIndex < GetFieldCount());
}
BOOL CSQLiteBase::IsEof()
{
	return m_bEof;
}