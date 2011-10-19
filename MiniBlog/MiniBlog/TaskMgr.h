#pragma once
#include "AutoLock.h"

typedef struct tag_TASK_PARAM
{
	DWORD dwTaskType;
	union
	{
		struct{
			TCHAR szUID[MAX_PATH];
			TCHAR szBlogID[MAX_PATH];
			TCHAR szContent[MAX_PATH];
		}post;
		struct
		{
			TCHAR szUserName[50];
			TCHAR szUserPwd[50];
		}user;
		struct
		{
			TCHAR szURL[1024];
		}ad;
	};
}TASK_PARAM,*LPTASK_PARAM;

typedef enum tag_TASK_ACTION
{
	ACT_LOGIN_SINA,
	ACT_FOLLOW_SINA,
	ACT_CHECK_FOLLOWER_SINA,
	ACT_POST_SINA,
	ACT_COMMENT_SINA,
	ACT_FORWARD_SINA,

	ACT_CLICK_AD,
		
	ACT_NULL
}TASK_ACTION;
typedef struct _tagUserInfo
{
	TCHAR szName[50];
	TCHAR szUID[30];
	TCHAR szPWD[50];
	DWORD dwStatus;
}USERINFO,*LPUSERINFO;
class CTaskProcessBase;  
class CTaskMgr
{
public:
	CTaskMgr(void);
	~CTaskMgr(void);
public:
	void SetSvr(CTaskProcessBase * pSvr){m_pSvr = pSvr;};
	void GetTask();
private:
	BOOL CheckElapsed();
private:
	DWORD m_dwLastTick;
	CTaskProcessBase *m_pSvr;


};

