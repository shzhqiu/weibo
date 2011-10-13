#pragma once
#include "TaskMgr.h"
#include "WebBrowser.h"

class CTaskProcseeBase
{
public:
	virtual HRESULT ProcessTask(LPTASK_PARAM lpTaskParam)= 0;
};
class CWeiboServiceBase : public CTaskProcseeBase,
						  public CWebBrowserBase
{

};

