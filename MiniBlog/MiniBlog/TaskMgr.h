#pragma once
typedef struct tag_TASK_PARAM
{
	DWORD dwTaskType;
	TCHAR szUID[MAX_PATH];
	TCHAR szBlogID[MAX_PATH];
	TCHAR szContent[MAX_PATH];
}TASK_PARAM,*LPTASK_PARAM;

typedef enum tag_TASK_ACTION
{
	ACT_LOGIN_SINA,
	ACT_FOLLOW_SINA,
	ACT_CHECK_FOLLOWER_SINA,
	ACT_POST_SINA,
	ACT_REPLY_SINA,
	ACT_REPOST_SINA,

	ACT_END
}TASK_ACTION;

class WeiboServiceBase;  
class CTaskMgr
{
public:
	CTaskMgr(void);
	~CTaskMgr(void);
public:
	void SetSvr(WeiboServiceBase * pSrv){m_pSrv = pSrv;};
	void GetTask();
private:
	BOOL CheckElapsed();
private:
	DWORD m_dwLastTick;
	WeiboServiceBase *m_pSrv;
	TASK_PARAM m_taskParam;

};

