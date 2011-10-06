#pragma once
#include "./sqlite/inc/sqlite3.h"
class CSQLiteTool
{
public:
	CSQLiteTool(void);
	~CSQLiteTool(void);
public:
	HRESULT OpenDB(char* lppath);
	HRESULT AddFans(char *lpUser,char* pwd);
private:
	HRESULT InitDB();
	HRESULT CheckVersion();
private:
	sqlite3 *m_db; 
};

