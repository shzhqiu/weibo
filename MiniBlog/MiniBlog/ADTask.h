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
	BOOL	IsThreadClose(){return m_bClose;}
private:
	HRESULT ProcessTask();
	HRESULT PostAD();
	HRESULT ClickAD();
	HRESULT GetAD();
private:
	HANDLE m_hThread;
	HANDLE m_hEvent; 
	TASK_PARAM m_taskParam;
	BOOL       m_bClose;
	CCritSec   m_Lock;


};

