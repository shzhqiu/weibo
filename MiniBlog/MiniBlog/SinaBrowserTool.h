#pragma once
#include "simplebrowser.h"
enum ERROR_CODE
{
	SINA_OK = 0,
	SINA_PWD_ERROR,
	SINA_LOGINING,
	SINA_NO_WEIBO,

	SINA_NONE

};
typedef enum tag_MINIBLOG_ACTION
{
	ACT_LOGIN_SINA,
	ACT_FOLLOW_SINA,
	ACT_CHECK_FOLLOWER_SINA,
	ACT_POST_SINA,
	ACT_REPLY_SINA,
	ACT_REPOST_SINA,

	ACT_END
}MINIBLOG_ACTION;

class SinaBrowserTool :	public BrowserConcrete
{
public:
	SinaBrowserTool(void);
	~SinaBrowserTool(void);

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
	void AddTask(MINIBLOG_ACTION task);
	void SetAction(MINIBLOG_ACTION act){m_CurTask = act;};
	void Login(LPCTSTR lpUserName,LPCTSTR lpPwd);
private:
	MINIBLOG_ACTION GetAction(){return m_CurTask;};
	HRESULT  CheckLoginStatus(CString URL);
public:

private:
	MINIBLOG_ACTION m_CurTask;

	DWORD          m_ActionStatus;

};

