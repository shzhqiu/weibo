// AutoUpdate.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include ".\autoupdate.h"
#include <tlhelp32.h>
#include "ZLibWrap/ZLibWrap.h"
#pragma comment(lib,"ZLibWrap/ZLibWrap.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  MAIN_APP_SECRET     _T("WEIBOJUNTUAN_CENTMIND_COM")


// CAutoUpdateApp

BEGIN_MESSAGE_MAP(CAutoUpdateApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoUpdateApp 构造

CAutoUpdateApp::CAutoUpdateApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAutoUpdateApp 对象

CAutoUpdateApp theApp;


// CAutoUpdateApp 初始化
extern CString GetModuleDirectory();

BOOL CAutoUpdateApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CString csServerURL = GetCommandLine();
	{
		int iIndex = csServerURL.Find(_T(" "));
		if(iIndex >= 0)
		{
			csServerURL = csServerURL.Mid(iIndex+1); 
		}
	}

	CAutoUpdateDlg dlg;
	dlg.m_csDownloadURL = csServerURL;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}
	if(dlg.m_bFileOK)  //表示文件下载下来了
	{
		CString csFile = GetModuleDirectory()+UPDATE_PACKAGE;
		StartInstall(csFile);
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

void CAutoUpdateApp::LaunchMainApp(LPCTSTR lpFileName)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	TCHAR temp = _T('\"');
	CString strCmdLine;
	strCmdLine = CString(temp) + lpFileName + CString(temp);
	TCHAR* p_CmdLine;
	p_CmdLine=strCmdLine.GetBuffer(strCmdLine.GetLength());
	if( !CreateProcess( NULL, // No module name (use command line). 
		p_CmdLine,		  // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		//AfxMessageBox( "CreateProcess failed." );
		return;
	}
	// Wait until child process exits.
	//WaitForSingleObject( pi.hProcess, INFINITE);
	// Close process and thread handles. 
	//CloseHandle( pi.hProcess );
	//CloseHandle( pi.hThread );
}

BOOL CloseMainApp()
{
	HANDLE   hSem   =   CreateSemaphore(NULL,   1,   1,   MAIN_APP_SECRET);   

	int nCnt = 0;
	//   信号量已存在？   
	//   信号量存在，则程序已有一个实例运行   
	if(GetLastError()   ==   ERROR_ALREADY_EXISTS)   
	{   
		//   关闭信号量句柄   
		CloseHandle(hSem);   
		//   寻找先前实例的主窗口   
		HWND   hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);   
		while   (::IsWindow(hWndPrevious))   
		{   
			//   检查窗口是否有预设的标记?   
			//   有，则是我们寻找的主窗   
			if (::GetProp(hWndPrevious,   MAIN_APP_SECRET))   
			{   
				PostMessage(hWndPrevious,WM_CLOSE,NULL,NULL);
				Sleep(300);
				// find again
				hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);
				nCnt ++;
				if (nCnt > 10)
				{
					AfxMessageBox(_T("下载完成，即将重新启动微博军团，开始升级！"),MB_ICONINFORMATION);
					break;
				}
				continue;
			}   
			//   继续寻找下一个窗口   
			hWndPrevious   =   ::GetWindow(hWndPrevious,GW_HWNDNEXT); 
		}   

	} 
	else
		CloseHandle(hSem);
	return TRUE;
}
void CAutoUpdateApp::StartInstall(LPCTSTR lpFileName)
{
	CString strModuleDir = GetModuleDirectory();
	CString strMainApp = strModuleDir + MAIN_APP_NAME;
	CloseMainApp();
	DeleteFile(strMainApp.GetBuffer());

	ZWZipExtract(lpFileName,strModuleDir.GetBuffer());
	DeleteFile(lpFileName);
	LaunchMainApp(strMainApp.GetBuffer());

	return;

}

void CAutoUpdateApp::WaitForProcessOK(LPCTSTR lpProcName)
{
	HANDLE hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 processInfo;
	processInfo.dwSize=sizeof(PROCESSENTRY32);
	while(Process32Next(hSnapShot,&processInfo)!=FALSE)
	{
		if(_tcsicmp(processInfo.szExeFile,lpProcName) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processInfo.th32ProcessID);
			TerminateProcess((HANDLE)(hProcess),0);
			CloseHandle(hProcess);
			break;
		}		
	}
	CloseHandle(hSnapShot);
}
