#pragma once
#include "AutoLock.h"

typedef struct tag_TASK_PARAM
{
	DWORD dwTaskType;
	TCHAR szClientID[33];
	union
	{
		struct{
			TCHAR szUID[20];
			TCHAR szToUID[15];
			TCHAR szBlogID[33];
			TCHAR szContent[MAX_PATH*5];
		}post;
		struct
		{
			TCHAR szUID[20];
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
	ACT_SINA_UPLOAD_HEADIMG,

	ACT_CLICK_AD,
	ACT_POST_AD,
	ACT_GET_AD,
	ACT_POST_S_INFO,
	ACT_POST_M_INFO,
	ACT_POST_C_STATUS,

	ACT_GET_SINA_FOLLOW,
	ACT_GET_SINA_POST,
	ACT_GET_SINA_COMMENT,
	ACT_GET_SINA_REPLY,
	ACT_GET_SINA_REPOST,

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

