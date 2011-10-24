// AutoUpdate.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include ".\autoupdate.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
		int iIndex = csServerURL.Find(" ");
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
		CString csFile = GetModuleDirectory()+"update.zip";
		StartInstall(csFile);
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

void CAutoUpdateApp::StartInstall(LPCSTR lpFileName)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	char temp = '\"';
	CString strCmdLine;
	strCmdLine = CString(temp) + lpFileName + CString(temp);
	char* p_CmdLine;
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
	WaitForSingleObject( pi.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return;
}

void CAutoUpdateApp::WaitForProcessOK(LPCSTR lpProcName)
{
	HANDLE hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 processInfo;
	processInfo.dwSize=sizeof(PROCESSENTRY32);
	while(Process32Next(hSnapShot,&processInfo)!=FALSE)
	{
		if(stricmp(processInfo.szExeFile,lpProcName) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processInfo.th32ProcessID);
			TerminateProcess((HANDLE)(hProcess),0);
			CloseHandle(hProcess);
			break;
		}		
	}
	CloseHandle(hSnapShot);
}
