// AutoUpdate.cpp : ����Ӧ�ó��������Ϊ��
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


// CAutoUpdateApp ����

CAutoUpdateApp::CAutoUpdateApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAutoUpdateApp ����

CAutoUpdateApp theApp;


// CAutoUpdateApp ��ʼ��
extern CString GetModuleDirectory();

BOOL CAutoUpdateApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
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
		// TODO: �ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}
	if(dlg.m_bFileOK)  //��ʾ�ļ�����������
	{
		CString csFile = GetModuleDirectory()+UPDATE_PACKAGE;
		StartInstall(csFile);
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
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
			if (::GetProp(hWndPrevious,   MAIN_APP_SECRET))   
			{   
				PostMessage(hWndPrevious,WM_CLOSE,NULL,NULL);
				Sleep(300);
				// find again
				hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);
				nCnt ++;
				if (nCnt > 10)
				{
					AfxMessageBox(_T("������ɣ�������������΢�����ţ���ʼ������"),MB_ICONINFORMATION);
					break;
				}
				continue;
			}   
			//   ����Ѱ����һ������   
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
