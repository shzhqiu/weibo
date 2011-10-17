#pragma once
#include "WeiboServiceBase.h"

enum ERROR_CODE
{
	SINA_OK = 0,
	SINA_PWD_ERROR,
	SINA_LOGINING,
	SINA_NO_WEIBO,

	SINA_NONE

};
class CSinaSvr:public CWeiboServiceBase
{
public:
	CSinaSvr(HWND hWnd);
	~CSinaSvr(void);

	virtual HRESULT STDMETHODCALLTYPE GetExternal(IDispatch **ppDispatch);
	virtual void OnDocumentCompleted(LPCTSTR lpURL);

	// weibo service base
public:
	virtual HRESULT ProcessTask(LPTASK_PARAM lpTaskParam);
	// browserbase
public:
	virtual HWND GetHWND(){return m_hWnd;}


public:
	BOOL  CreateFromControl(CWnd *pParentWnd,UINT nID);


private:
	//void AddTask(TASK_ACTION task);
	void SetAction(TASK_ACTION act){m_CurTask = act;};
	TASK_ACTION GetAction(){return m_CurTask;};
	DWORD GetActionStatus(){return m_ActionStatus;}
	void  SetActionStatus(DWORD dwStatus){m_ActionStatus=dwStatus;}

	void Login(LPCTSTR lpUserName,LPCTSTR lpPwd);
	void PostWeibo(LPCTSTR lpContent);
	void PostWeibo1(LPCTSTR lpContent);
	void Follow(LPCTSTR  lpUID);
	void Forward(LPCTSTR  lpMid,LPCTSTR lpUID,LPCTSTR lpReason);
	void Comment(LPCTSTR lpMid,LPCTSTR lpUID,LPCTSTR lpContent);


	HRESULT CheckLoginStatus(CString URL);
	HRESULT GetUID();

private:
	HWND m_hWnd;
	TASK_ACTION m_CurTask;
	CString		m_strCookie;

	DWORD          m_ActionStatus;
	CString		   m_strCurURL;
	TCHAR          m_szCurUID[MAX_PATH];

};
