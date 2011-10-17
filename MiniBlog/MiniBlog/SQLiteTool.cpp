#include "StdAfx.h"
#include "SQLiteTool.h"

#define DB_PATH_MINIBLOG "client.db"
#define DB_VERSION        20111006

CSQLiteTool::CSQLiteTool(void)
{
}

void testsqlitedb()
{
#if 0


	sqlite3 *db; 
	sqlite3_stmt * stmt; 
	const char *zTail;

	//打开数据库 
	int r = sqlite3_open("mysqlite.db",&db) ;
	if(r){ 
		printf("%s",sqlite3_errmsg(db)); 
	}

	//创建Table 
	sqlite3_prepare(db, 
		"CREATE TABLE players ( ID INTEGER PRIMARY KEY, name TEXT, age INTERER );", 
		-1,&stmt,&zTail); 
	sqlite3_step(stmt); 
	//sqlite3_reset(stmt);

	sqlite3_finalize(stmt);

	//插入数据 
	r = sqlite3_prepare(db, 
		"INSERT INTO players (name,age) VALUES(?,?);", 
		-1,&stmt,&zTail);

	char str[] = "Kevin"; 
	int n = 23; 
	r = sqlite3_bind_text(stmt,1,str,-1,SQLITE_STATIC); 
	r = sqlite3_bind_int(stmt,2,n); 
	r = sqlite3_step(stmt); 
	if( r!=SQLITE_DONE){ 
		printf("%s",sqlite3_errmsg(db)); 
	} 
	sqlite3_reset(stmt);

	//插入第二个数据 
	char str2[] = "Jack"; 
	int n2 = 16; 
	sqlite3_bind_text(stmt,1,str2,-1,SQLITE_STATIC); 
	sqlite3_bind_int(stmt,2,n2); 
	r = sqlite3_step(stmt); 

	if( r!=SQLITE_DONE){ 
		printf("%s",sqlite3_errmsg(db)); 
	} 
	sqlite3_finalize(stmt);

	//查询所有数据 
	sqlite3_prepare(db, 
		"SELECT ID, name, age FROM players ORDER BY ID;", 
		-1,&stmt,&zTail); 
	r = sqlite3_step(stmt); 
	int number; 
	int id; 
	const unsigned char * name; 
	while( r == SQLITE_ROW ){ 
		id = sqlite3_column_int( stmt, 0 ); 
		name = sqlite3_column_text( stmt,1 ); 
		number = sqlite3_column_int( stmt, 2 ); 
		printf("ID: %d  Name: %s  Age: %d /n",id,name,number); 
		sqlite3_step(stmt); 
	} 
	sqlite3_finalize(stmt);

	//关闭数据库 
	sqlite3_close(db); 
#endif
}
CSQLiteTool::~CSQLiteTool(void)
{
	sqlite3_close(m_db); 

}
HRESULT CSQLiteTool::AddFans(char *lpUser,char* pwd)
{
	sqlite3_stmt * stmt; 
	const char *zTail;
	HRESULT r = sqlite3_prepare(m_db, 
		"INSERT INTO subfans (name,pwd,lastlogin,status) VALUES(?,?,'2011-11-11 11:11:11',1);", 
		-1,&stmt,&zTail);


	int n = MAX_PATH; 
	r = sqlite3_bind_text(stmt,1,lpUser,-1,SQLITE_STATIC); 
	r = sqlite3_bind_text(stmt,2,pwd,-1,SQLITE_STATIC); 
	r = sqlite3_step(stmt); 
	if( r!=SQLITE_DONE){ 
		printf("%s",sqlite3_errmsg(m_db)); 
	} 
	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);
	return r;

}

HRESULT CSQLiteTool::OpenDB(char* lppath)
{
	int r = sqlite3_open(DB_PATH_MINIBLOG,&m_db) ;
	if( r!= SQLITE_OK){ 
		printf("%s",sqlite3_errmsg(m_db)); 
		return r;
	}
		InitDB();
	return r;
}
HRESULT CSQLiteTool::CheckVersion()
{
	return S_FALSE;
}
HRESULT CSQLiteTool::InitDB()
{
	if(!m_db)
		return E_FAIL;
	static char initsql[][MAX_PATH]=
	{
		{"CREATE TABLE subfans( ID INTEGER PRIMARY KEY, name VCHAR(255), pwd VCHAR(255),lastlogin DATETIME,status INTEGER);"}

	};
	sqlite3_stmt * stmt; 
	const char *zTail;

	int nTableCnt = sizeof(initsql) / sizeof(initsql[0]);
	

	for (int i = 0 ; i < nTableCnt; ++i)
	{

	
	sqlite3_prepare(m_db, 
		initsql[i], 
		-1,&stmt,&zTail); 
	sqlite3_step(stmt); 
	}
	sqlite3_finalize(stmt);
}
