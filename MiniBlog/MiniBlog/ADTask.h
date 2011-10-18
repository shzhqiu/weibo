#pragma once
#include "weiboservicebase.h"
class CADTask :
	public CTaskProcessBase
{
public:
	CADTask(void);
	~CADTask(void);

public:
	HRESULT AddTask(LPTASK_PARAM lpTaskParam);
	static DWORD ThreadProc (LPVOID lpdwThreadParam );
	BOOL	GetThreadStatus(){return m_bClose;}
private:
	HRESULT ProcessTask();
private:
	HANDLE m_hThread;
	HANDLE m_hEvent; 
	TASK_PARAM m_taskParam;
	BOOL       m_bClose;
	CCritSec   m_Lock;


};

