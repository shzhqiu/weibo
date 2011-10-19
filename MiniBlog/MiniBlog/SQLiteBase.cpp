#include "StdAfx.h"
#include "SQLiteBase.h"

#define DB_PATH_MINIBLOG _T("client.db")
#define DB_VERSION        20111006

CSQLiteBase::CSQLiteBase(void)
{
	m_bEof = TRUE;
	m_pSTMT = NULL;
	m_pSQLiteDB = NULL;
}

CSQLiteBase::~CSQLiteBase(void)
{
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
		{_T("CREATE TABLE subfans( ID INTEGER PRIMARY KEY, uid VCHAR(255), name VCHAR(255), pwd VCHAR(255),lastlogin DATETIME,status INTEGER,type INTEGER);")},
		{_T("CREATE TABLE mainID( ID INTEGER PRIMARY KEY, uid VCHAR(255),lastlogin DATETIME,status INTEGER,type INTEGER);")}

	};

	int nTableCnt = sizeof(initsql) / sizeof(initsql[0]);
	
	BeginTrans();
	for (int i = 0 ; i < nTableCnt; ++i)
	{
		ExecuteSQL(initsql[i]);
	}
	CommitTrans();

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