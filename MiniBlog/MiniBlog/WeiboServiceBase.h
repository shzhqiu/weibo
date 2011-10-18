#pragma once
#include "TaskMgr.h"
#include "WebBrowser.h"

class CTaskProcessBase
{
public:
	virtual HRESULT AddTask(LPTASK_PARAM lpTaskParam)= 0;
};
class CWeiboServiceBase : public CTaskProcessBase,
						  public CWebBrowserBase
{

};

