
// MiniBlog.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MiniBlog.h"
#include "MiniBlogDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void SetRegistKey()
{
	TCHAR regname[]=_T("MIME\\Database\\Content Type\\"); 
		HKEY hAppKey = NULL;
        HKEY hSoftKey = NULL;
        HKEY hCompanyKey = NULL;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, regname, 0, KEY_WRITE|KEY_READ,&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, _T("application/json"), 0, REG_NONE,
                REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
                   &hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			TCHAR lpszValue[]=_T("{25336920-03F9-11cf-8FD0-00AA00686F13}");
			RegSetValueEx(hCompanyKey, _T("CLSID"), NULL, REG_SZ,
                   (LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
			DWORD dwVaule = 0x80000;//
			RegSetValueEx(hCompanyKey, _T("Encoding"), NULL, REG_DWORD,(LPBYTE) &dwVaule, sizeof(DWORD));

		}
	}
	if (hSoftKey != NULL)
    {
             RegCloseKey(hSoftKey);
    }   
 
    if (hCompanyKey != NULL)
    {
           RegCloseKey(hCompanyKey);
    }   
}


// CMiniBlogApp

BEGIN_MESSAGE_MAP(CMiniBlogApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMiniBlogApp construction

CMiniBlogApp::CMiniBlogApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMiniBlogApp object

CMiniBlogApp theApp;


// CMiniBlogApp initialization

BOOL CMiniBlogApp::InitInstance()
{
	HANDLE   hSem   =   CreateSemaphore(NULL,   1,   1,   APP_NAME);   

	//   �ź����Ѵ��ڣ�   
	//   �ź������ڣ����������һ��ʵ������   
	if(GetLastError()   ==   ERROR_ALREADY_EXISTS)   
	{   
		//   �ر��ź������   
		CloseHandle(hSem);   
		//   Ѱ����ǰʵ����������   
		HWND   hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);   
		while   (::IsWindow(hWndPrevious))   
		{   
			//   ��鴰���Ƿ���Ԥ��ı��?   
			//   �У���������Ѱ�ҵ�����   
			if (::GetProp(hWndPrevious,   m_pszExeName))   
			{   
				//   ����������С������ָ����С   
				if   (::IsIconic(hWndPrevious))   
					::ShowWindow(hWndPrevious,SW_RESTORE);   

				//   ����������   
				::SetForegroundWindow(hWndPrevious);   

				//   �������ĶԻ��򼤻�   
				::SetForegroundWindow(   
					::GetLastActivePopup(hWndPrevious));   

				//   �˳���ʵ��   
				return   FALSE;   
			}   
			//   ����Ѱ����һ������   
			hWndPrevious   =   ::GetWindow(hWndPrevious,GW_HWNDNEXT); 
		}   

		return   FALSE;   
	} 

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	SetRegistKey();

	CMiniBlogDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

