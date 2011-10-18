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
private:
	HRESULT ProcessTask(LPTASK_PARAM lpTaskParam);
};

