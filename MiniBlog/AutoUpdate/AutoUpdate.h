// AutoUpdate.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CAutoUpdateApp:
// �йش����ʵ�֣������ AutoUpdate.cpp
//

class CAutoUpdateApp : public CWinApp
{
public:
	CAutoUpdateApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	void StartInstall(LPCSTR lpFileName);
private:
	void WaitForProcessOK(LPCSTR lpProcName);
};

extern CAutoUpdateApp theApp;
