// AutoUpdateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include ".\autoupdatedlg.h"
#include <tlhelp32.h>
#include <afxsock.h>		// MFC socket extensions
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoUpdateDlg �Ի���
CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwDownloadedSize = 0;
	m_dwTotalSize = 0;
	m_bFileOK = FALSE;
}

void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProcess);
	DDX_Control(pDX, IDC_PROCESSVALUE, m_ctPercentValue);
}

BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_POSMESSAGE,OnPositionMessage)
	ON_MESSAGE(WM_ENDMESSAGE,OnEndMessage)
END_MESSAGE_MAP()


// CAutoUpdateDlg ��Ϣ�������

BOOL CAutoUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(0,0,DoRun,this,0,&dwThreadID);
	CloseHandle(hThread);
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoUpdateDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CAutoUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// CMsgDialog ��Ϣ�������

LRESULT CAutoUpdateDlg::OnPositionMessage(WPARAM wParam,LPARAM lParam)
{
	int iPer = 0;
	if(m_dwTotalSize != 0) iPer = (m_dwDownloadedSize*100)/m_dwTotalSize;
	CString csPercent; csPercent.Format(_T("%d"),iPer); csPercent = csPercent+_T("%");
	m_ctPercentValue.SetWindowText(csPercent);
	m_ctlProcess.SetRange(0,100);
	m_ctlProcess.SetPos(iPer);
	return 1;
}
#define  MAIN_APP_SECRET     _T("WEIBOJUNTUAN_CENTMIND_COM")

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
				AfxMessageBox(_T("dddadafasdfas"),MB_RETRYCANCEL);
				// find again
				hWndPrevious   =   ::GetWindow(::GetDesktopWindow(),GW_CHILD);
				nCnt ++;
				if (nCnt > 10)
				{
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
LRESULT CAutoUpdateDlg::OnEndMessage(WPARAM wParam,LPARAM lParam)
{
	CloseMainApp();
	OnCancel();
	return 0;
}

void CAutoUpdateDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

 DWORD WINAPI CAutoUpdateDlg::DoRun(LPVOID lParam)
{
	CAutoUpdateDlg * pThis = (CAutoUpdateDlg *)lParam;
	pThis->DoDownLoadThread();
	return TRUE;
}

CString GetModuleDirectory()
{
	CString strFilePath;
	GetModuleFileName(AfxGetInstanceHandle(),strFilePath.GetBuffer(MAX_PATH),MAX_PATH);
	strFilePath.ReleaseBuffer();
	int iStart = strFilePath.ReverseFind('\\');
	strFilePath = strFilePath.Mid(0,iStart + 1);
	return strFilePath;	
}

DWORD CAutoUpdateDlg::DoDownLoadThread(void)
{
	CString csCurDir = GetModuleDirectory()+UPDATE_PACKAGE;
	if(DownloadFile(m_csDownloadURL,csCurDir))
	{
		m_bFileOK = TRUE;
	}
	return PostMessage(WM_ENDMESSAGE,0,0);
}

#define BLOCKSIZE 1024
BOOL CAutoUpdateDlg::DownloadFile(LPCTSTR lpURL,LPCTSTR lpDestFile)
{
	CFile cUdpFile; 
	if(!cUdpFile.Open(lpDestFile,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary|CFile::shareDenyWrite))
		return FALSE;
	BOOL bResult = FALSE;
	CInternetSession sessionDownload;
	try
	{
		CHttpFile* pFile = (CHttpFile*)sessionDownload.OpenURL(_T("http://weibodata-update.stor.sinaapp.com/MiniBlog.zip"),1,INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_DONT_CACHE);
		CString   query = _T("");
		pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,query);
		long file_len=_ttol(query);
		m_dwTotalSize = file_len;
		PostMessage(WM_POSMESSAGE,0,0);
		DWORD dwStatus;
		if (pFile->QueryInfoStatusCode(dwStatus))
		{
			if (dwStatus == 200)
			{
				pFile->SetReadBufferSize(10240);
				if (TRUE)
				{
					DWORD dwLen = 0;
					char buf[BLOCKSIZE];
					while (TRUE)
					{
						DWORD dwTemp = pFile->Read(buf,BLOCKSIZE);
						if (dwTemp)
						{
							cUdpFile.Write(buf,dwTemp);
						}
						m_dwDownloadedSize += dwTemp;
						PostMessage(WM_POSMESSAGE,0,0);
						if (dwTemp < BLOCKSIZE)
						{
							break;
						}
					}
				}
			}
		}
		pFile->Close();
		bResult = TRUE;
	}
	catch(CInternetException* pException)
	{
		pException->Delete();
		return bResult;
	}
	sessionDownload.Close();
	cUdpFile.Close();
	return bResult;
}
