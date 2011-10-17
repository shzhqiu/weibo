#pragma once
#include "simplebrowser.h"
#include "TaskMgr.h"

enum ERROR_CODE
{
	SINA_OK = 0,
	SINA_PWD_ERROR,
	SINA_LOGINING,
	SINA_NO_WEIBO,

	SINA_NONE

};


class CSinaSvr :	public WeiboServiceBase1
{
public:
	CSinaSvr(void);
	~CSinaSvr(void);

public:

	virtual bool OnBeforeNavigate2(CString URL,
		CString frame,
		void    *post_data,int post_data_size,
		CString headers);
	virtual void OnDocumentComplete(CString URL);
	virtual void OnDownloadBegin();
	virtual void OnProgressChange(int progress,int progress_max);
	virtual void OnDownloadComplete();
	virtual void OnNavigateComplete2(CString URL);
	virtual void OnStatusTextChange(CString text);
	virtual void OnTitleChange(CString text);
public:
	//void AddTask(TASK_ACTION task);
	void SetAction(TASK_ACTION act){m_CurTask = act;};
	void Login(LPCTSTR lpUserName,LPCTSTR lpPwd);
	void PostWeibo(LPCTSTR lpContent);
	void PostWeibo1(LPCTSTR lpContent);
	void Follow(LPCTSTR  lpUID);
	void Forward(LPCTSTR  lpMid,LPCTSTR lpUID,LPCTSTR lpReason);
	void Comment(LPCTSTR lpMid,LPCTSTR lpUID,LPCTSTR lpContent);
protected:
	virtual HRESULT ProcessTask(LPTASK_PARAM lpTaskParam);
private:
	HRESULT  CheckLoginStatus(CString URL);
	TASK_ACTION GetAction(){return m_CurTask;}
public:

private:
	TASK_ACTION m_CurTask;
	CString		m_strCookie;

	DWORD          m_ActionStatus;
	CString		   m_strCurURL;
	TCHAR          m_szCurUID[MAX_PATH];

};

