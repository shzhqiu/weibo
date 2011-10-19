CallMemberFunc

// MiniBlogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniBlog.h"
#include "MiniBlogDlg.h"
#include "afxdialogex.h"
#include "SinaSvr.h"
#include "PubTool/PubTool.h"


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  GRID_ROW_COUNT 3

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
	m_pDB = new CSQLiteBase();
	m_pTaskMgr = new CTaskMgr();
	m_pSinaSQL = new CSinaSQLTool(m_pDB);
}
CMiniBlogDlg::~CMiniBlogDlg()
{
	SafeDelete(m_pTaskMgr);
	SafeDelete(m_pSinaSvr);
	SafeDelete(m_pSinaSQL);
	SafeDelete(m_pDB);
	m_vtUserList.clear();

};
BOOL CMiniBlogDlg::IsUserAdded(LPCTSTR lpUserName)
{
	std::vector<USERINFO>::iterator end = m_vtUserList.end();
	std::vector<USERINFO>::iterator beg = m_vtUserList.begin();
	for (  ; beg != end ; beg++ ) 
	{ 
		if (_tcsicmp(lpUserName,beg->szName) == 0)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}
void CMiniBlogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_edtUsername);
	DDX_Control(pDX, IDC_EDIT_USERPWD, m_edtUserPwd);
	DDX_Control(pDX, IDC_GRID_CTRL, m_Grid);
	DDX_Control(pDX, IDC_BUTTON_ADD_USER, m_btnAddUser);
}

BEGIN_MESSAGE_MAP(CMiniBlogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1_TEST, &CMiniBlogDlg::OnBnClickedButton1Test)
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CMiniBlogDlg::OnBnClickedButtonAddUser)
	ON_WM_TIMER()
	ON_WM_DESTROY()

	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_LOGIN_STATUS, OnLoginStatus)

END_MESSAGE_MAP()

BOOL CMiniBlogDlg::Init()
{
	if(!m_pDB)
		return FALSE;
	m_pDB->OpenDB();
	InitClientID();

	return S_OK;
}

BOOL CMiniBlogDlg::InitGrid() 
{
	UpdateData();

	m_Grid.SetVirtualMode(FALSE);
	m_Grid.SetEditable(FALSE);
	m_Grid.EnableDragAndDrop(FALSE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_Grid.SetFixedColumnSelection(FALSE);
	m_Grid.SetFixedRowSelection(FALSE);
	m_Grid.EnableColumnHide();
	m_Grid.SetHeaderSort(FALSE);
	m_Grid.EnableTitleTips(TRUE);
/*	m_Grid.SetGridLines(GVL_NONE);
	m_Grid.SetGridLines(GVL_HORZ);
	m_Grid.SetGridLines(GVL_VERT);
	*/
	m_Grid.SetGridLines(GVL_BOTH);
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_Grid.SetDefCellHeight(25);

	
	{
		int nFixCols = 1;
		int nFixRows = 1;
		int nCols = GRID_ROW_COUNT;
		int nRows = 1;


	

		TRY {
			m_Grid.SetRowCount(nRows);
			m_Grid.SetColumnCount(nCols);
			m_Grid.SetFixedRowCount(nFixRows);
			m_Grid.SetFixedColumnCount(nFixCols);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			return FALSE;
		}
		END_CATCH

			int row = 0;
			// fill rows/cols with text
			//for ( row = 0; row < m_Grid.GetRowCount(); row++)
			{
					GV_ITEM Item;

					Item.mask = GVIF_TEXT;
					Item.row = row;
				for (int col = 0; col < GRID_ROW_COUNT; col++)
				{ 
					
						
					CString str;
					Item.col = col;
					switch(col)
					{
					case 0:
						Item.strText = _T("序号");
						m_Grid.SetColumnWidth(col,50);
						break;
					case 1:
						Item.strText = _T("帐号");
						m_Grid.SetColumnWidth(col,120);
						break;
					case 2:
						Item.strText = _T("状态");
						m_Grid.SetColumnWidth(col,80);
						break;
					}

					if (rand() % 10 == 1)
					{
						COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
						Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
						Item.crFgClr = RGB(255,0,0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
						Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
					}
					m_Grid.SetItem(&Item);
				}
			}
	}
	m_Grid.SetItemFormat(0,1,DT_CENTER | DT_VCENTER);
	//m_Grid.setf

	//m_Grid.GetDefaultCell(FALSE,FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
	//m_Grid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER);

	// Having weird Autosize problems with CE 2.11 - possibly just an emulation problem
	//m_Grid.AutoSize();


	UpdateData(FALSE);
	return TRUE;
}
BOOL CMiniBlogDlg::InitUI()
{
	if (!m_pSinaSvr)
		return FALSE;
	m_pSinaSvr = new CSinaSvr(GetSafeHwnd());
	m_pADsvr   = new CADTask();
	m_pTaskMgr->SetSvr(m_pSinaSvr);
	m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
	InitGrid();
	//m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
#ifdef _DEBUG
	//CString strName = _T("fortestonlya@sina.com");
	//CString strPwd = _T("fortestonlya");
	m_edtUsername.SetWindowText(_T("shzhqiu@hotmail.com"));
	m_edtUserPwd.SetWindowText(_T("93732717"));


#endif // _DEBUG


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
	//SetTimer(TIMER_AUTO_START,1000*60,NULL);
	m_pTaskMgr->GetTask();
#if 0


	TASK_PARAM tp = {0};
	/*
	tp.dwTaskType = ACT_LOGIN_SINA;
	_tcscpy(tp.user.szUserName,_T("shzhqiu@hotmail.com"));
	_tcscpy(tp.user.szUserPwd,_T("93732717"));

	m_pSinaSvr->AddTask(&tp);
	*/
	tp.dwTaskType = ACT_CLICK_AD;
	_tcscpy(tp.ad.szURL,_T("http://taourl.com/p9i94"));
	
	m_pADsvr->AddTask(&tp);
#endif
	//m_pSinaSQL->AddFans(_T("1234567"),_T("test"),_T("pwd"));
	// TODO: Add your control notification handler code here
}


void CMiniBlogDlg::OnBnClickedButtonAddUser()
{
	
	CString strName;
	m_edtUsername.GetWindowText(strName);
	CString strPwd;
	m_edtUserPwd.GetWindowText(strPwd);

	if (strName.GetLength() <10 || strPwd.GetLength() <= 5)
	{
		AfxMessageBox(_T("请正确输入帐号和密码"),MB_ICONINFORMATION);
		return;
	}
	if (IsUserAdded(strName.GetBuffer()))
	{
		TCHAR szTip[MAX_PATH] = {};
		_stprintf(szTip,_T("%s 已经添加过了。"),strName.GetBuffer());
		AfxMessageBox(szTip,MB_ICONINFORMATION);
		return;
	}
	AddFansToGrid(strName,strPwd);
	EnableAddUser(FALSE);

	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_LOGIN_SINA;
	_tcscpy(tp.user.szUserName,strName.GetBuffer());
	_tcscpy(tp.user.szUserPwd,strPwd.GetBuffer());

	m_pSinaSvr->AddTask(&tp);

	// TODO: Add your control notification handler code here
}
void CMiniBlogDlg::EnableAddUser(BOOL bEnable)
{
	m_btnAddUser.EnableWindow(bEnable);
	m_edtUsername.EnableWindow(bEnable);
	m_edtUserPwd.EnableWindow(bEnable);

}
void CMiniBlogDlg::AddUserToDB(LPUSERINFO lpUI)
{
	if (!m_pSinaSQL || !lpUI)
		return;
	m_pSinaSQL->AddFans(lpUI->szUID,lpUI->szName,lpUI->szPWD);
}
void CMiniBlogDlg::AddFansToGrid(LPCTSTR lpName,LPCTSTR lpPWD)
{
	int nRow = m_Grid.GetRowCount();
	m_Grid.InsertRow(_T("test"));
	GV_ITEM Item;
	CString str;
	USERINFO uinfo;
	_tcscpy(uinfo.szName,lpName);
	_tcscpy(uinfo.szPWD,lpPWD);
	m_vtUserList.push_back(uinfo);
	Item.mask = GVIF_TEXT;
	Item.row = nRow;
	for (int i = 0; i <GRID_ROW_COUNT;++i)
	{
		Item.col = i;
		switch(i)
		{
		case 0:
			str.Format(_T("%d"),nRow);
			Item.strText = str;
			break;
		case 1:
			Item.strText = lpName;
			break;
// 		case 2:
// 			Item.strText = lpPWD;
// 			break;
		case 2:
			Item.strText = _T("登录中...");
			//m_Grid.SetItemState(nRow,i, m_Grid.GetItemState(nRow,i) & ~GVIS_READONLY);
			//m_Grid.Invalidate();
			//m_Grid.SetCellType(nRow,i, RUNTIME_CLASS(CGridCellCheck));
			break;
		}
		COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
		Item.crFgClr = RGB(255,0,0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		m_Grid.SetItem(&Item);
		
	}
	m_Grid.Refresh();

}

void CMiniBlogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIMER_AUTO_START:
		m_pTaskMgr->GetTask();
		break;
	default:
		break;

	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMiniBlogDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(TIMER_AUTO_START);
	::RemoveProp(m_hWnd,  APP_NAME);
	TrayMessage( NIM_DELETE );	

	// TODO: Add your message handler code here
}


void CMiniBlogDlg::OnSize(UINT nType, int cx, int cy)
{
	if(nType == SIZE_MINIMIZED){
		ShowWindow(SW_HIDE);
	}
	else{
		CDialogEx::OnSize(nType, cx, cy);
	}	
	// TODO: Add your message handler code here
}
void CMiniBlogDlg::InitClientID()
{
	TCHAR szMAC[18];
	CPubTool::GetMAC(szMAC);
	TCHAR szMagic[100] = {0};
	_stprintf(szMagic,_T("www.centmind.com/www.weibojuntuan.com/%s"),szMAC);

	char cMagic[100] = {0};
	WideCharToMultiByte(CP_ACP,0,szMagic,-1,cMagic,100,NULL,NULL);
	char *cHash = CPubTool::GetMD5(cMagic);
	MultiByteToWideChar(CP_ACP,0,cHash,-1,m_szClientID,33);

	delete [] cHash;
	
}
BOOL CMiniBlogDlg::TrayMessage( DWORD dwMessage)
{

	CString sTip(_T("微博军团"));	

	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= GetSafeHwnd();
	tnd.uID			= IDR_MAINFRAME;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON;

	tnd.uCallbackMessage	= MYWM_NOTIFYICON;


	VERIFY( tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_MAINFRAME)) );

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;

	lstrcpyn(tnd.szTip, (LPCTSTR)sTip, sizeof(tnd.szTip)/sizeof(tnd.szTip[0]) );

	return Shell_NotifyIcon(dwMessage, &tnd);

}
LRESULT CMiniBlogDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// Open window when double click to the Systray Icon
	if(message == MYWM_NOTIFYICON){

		switch (lParam){
		case  WM_LBUTTONUP:
		case  WM_RBUTTONUP:
		case  WM_RBUTTONDBLCLK:
		case  WM_LBUTTONDBLCLK:
			switch (wParam)	{
			case IDR_MAINFRAME:
				{
					if(IsWindowVisible())
					{
						ShowWindow(SW_HIDE);
					}
					else{

						ShowWindow(SW_NORMAL);
						SetForegroundWindow();
						SetFocus();
					}
				}


				return TRUE;
				break;
			}
			break;
		}
	}	

	return CDialogEx::WindowProc(message, wParam, lParam);
}

int CMiniBlogDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	TrayMessage(NIM_ADD);
	::SetProp(m_hWnd,   APP_NAME,   (HANDLE)1);  
	return 0;
}


void CMiniBlogDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnClose();
}
void CMiniBlogDlg::SetUserStatus(USERINFO *pui,int nStatus)
{
	if (!pui)
		return;
	
	for (int i = 0; i < m_Grid.GetRowCount();++i)
	{
		CString strName = m_Grid.GetItemText(i,1);
		strName.Trim();
		if (_tcsicmp(strName.GetBuffer(),pui->szName) == 0)
		{
			switch(nStatus)
			{
			case  SINA_LOGIN_SUCCESS:
				{
					m_Grid.SetItemText(i,2,_T("登录成功"));
				}
				break;
			case   SINA_PWD_ERROR:
				{
					m_Grid.SetItemText(i,2,_T("密码错误"));
				}
				break;
			case   SINA_NO_WEIBO:
				{
					m_Grid.SetItemText(i,2,_T("未开通微博"));
				}
				break;
			default:
				{
					m_Grid.SetItemText(i,2,_T("登录失败，错误类型还不知道"));
				}
				break;
			}
			m_Grid.Refresh();
			return;
		}
	}
}

LRESULT CMiniBlogDlg::OnLoginStatus(WPARAM wParam, LPARAM lParam)
{
	//Do What ever you want
	int nCode = (int)lParam;
	USERINFO ui;
	if (wParam)
	{
		memcpy((PVOID)&ui,(PVOID)wParam,sizeof(ui));
	}
	switch(nCode)
	{

	case SINA_PWD_ERROR:
	case  SINA_LOGIN_SUCCESS:
		{
			SetUserStatus(&ui,nCode);
		}
		break;
	case SINA_NONE:
		{
			EnableAddUser(TRUE);

		}
		break;
	default:
		break;

	}
	EnableAddUser(TRUE);
	if (nCode == SINA_LOGIN_SUCCESS)
	{
		AddUserToDB(&ui);
	}
	return 0;
}