
// MiniBlogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniBlog.h"
#include "MiniBlogDlg.h"
#include "afxdialogex.h"
#include "SinaBrowserTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMiniBlogDlg dialog




CMiniBlogDlg::CMiniBlogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMiniBlogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDB = new CSQLiteTool();
	m_pSinaSvr = new SinaBrowserTool();
}

void CMiniBlogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_Username);
	DDX_Control(pDX, IDC_EDIT_USERPWD, m_UserPwd);
}

BEGIN_MESSAGE_MAP(CMiniBlogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1_TEST, &CMiniBlogDlg::OnBnClickedButton1Test)
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CMiniBlogDlg::OnBnClickedButtonAddUser)
END_MESSAGE_MAP()

BOOL CMiniBlogDlg::Init()
{
	if(!m_pDB)
		return FALSE;
	m_pDB->OpenDB(NULL);
	return S_OK;
}
BOOL CMiniBlogDlg::InitUI()
{
	if (!m_pSinaSvr)
		return FALSE;

	m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
	return S_OK;
}
// CMiniBlogDlg message handlers

BOOL CMiniBlogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	BOOL bRet = InitUI();
	bRet |= Init();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMiniBlogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMiniBlogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMiniBlogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMiniBlogDlg::OnBnClickedButton1Test()
{
	char szName[MAX_PATH] = {0};
	char szPwd[MAX_PATH]  = {0};

	m_pDB->AddFans("name","pwd");
	// TODO: Add your control notification handler code here
}


void CMiniBlogDlg::OnBnClickedButtonAddUser()
{
	CString strName;
	m_Username.GetWindowText(strName);
	CString strPwd;
	m_UserPwd.GetWindowText(strPwd);
	strName = _T("shzhqiu@hotmail.com");
	strPwd = _T("93732717");
	m_pSinaSvr->Login(strName.GetBuffer(strName.GetLength()),strPwd.GetBuffer(strPwd.GetLength()));

	// TODO: Add your control notification handler code here
}
