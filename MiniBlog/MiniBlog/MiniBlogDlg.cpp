
// MiniBlogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniBlog.h"
#include "MiniBlogDlg.h"
#include "afxdialogex.h"
#include "SinaSvr.h"
#include "PubTool/PubTool.h"
#include "afxinet.h"

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
	m_bSmartLogon = FALSE;
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
	std::deque<USERINFO>::iterator end = m_vtUserList.end();
	std::deque<USERINFO>::iterator beg = m_vtUserList.begin();
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
	DDX_Control(pDX, IDC_EDIT_AD, m_edtADURL);
	DDX_Control(pDX, IDC_BUTTON_POST_AD, m_btnADPost);
	DDX_Control(pDX, IDC_STATIC_ONLINE_CNT, m_stcOnlineCnt);
	DDX_Control(pDX, IDC_EDIT_MAIN_ID, m_ediMainID);
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

	ON_BN_CLICKED(IDC_BUTTON_SMART_LOGON, &CMiniBlogDlg::OnBnClickedButtonSmartLogon)
	ON_BN_CLICKED(IDC_BUTTON_POST_AD, &CMiniBlogDlg::OnBnClickedButtonPostAd)
	ON_EN_KILLFOCUS(IDC_EDIT_AD, &CMiniBlogDlg::OnEnKillfocusEditAd)
END_MESSAGE_MAP()

BOOL CMiniBlogDlg::Init()
{
	if(!m_pDB)
		return FALSE;
	m_pDB->OpenDB();
	InitClientID();

	return S_OK;
}
void CMiniBlogDlg::LoadDB()
{
	 m_pSinaSQL->PrepareGetFans();
	 do 
	 {
		 USERINFO ui;
		 HRESULT hr = m_pSinaSQL->GetFans(ui.szUID,ui.szName,ui.szPWD);
		 if (hr != S_OK)
			 break;
		 
		 AddFansToGrid(ui.szName,_T("准备登陆"));

		 m_vtUserList.push_back(ui);
		 m_vtSmartLogonList.push_back(ui);
	 } while (TRUE);
	 

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

	LoadDB();
	UpdateData(FALSE);
	return TRUE;
}
BOOL CMiniBlogDlg::InitUI()
{
	if (!m_pSinaSvr)
		return FALSE;
	m_pSinaSvr = new CSinaSvr(GetSafeHwnd());
	m_pCMSvr   = new CCommonTask();
	m_pTaskMgr->SetSvr(m_pSinaSvr);
	m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
	InitGrid();
	m_ToolTip.Create(this); 
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_STATIC_GETMAINID), _T("要显示的信息 ")); 
	//m_btnADPost.EnableWindow(FALSE);
	//m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
#ifdef _DEBUG
	//CString strName = _T("fortestonlya@sina.com");
	//CString strPwd = _T("fortestonlya");
	/*
	xb209x20@sina.com   891466324

	ll660l66@sina.com,2041236616
	*/
	//m_edtUsername.SetWindowText(_T("xb209x20@sina.com"));
	//m_edtUserPwd.SetWindowText(_T("891466324"));
	m_edtUsername.SetWindowText(_T("ll660l66@sina.com"));
	m_edtUserPwd.SetWindowText(_T("2041236616"));
	m_ediMainID.SetWindowText(_T("2400232192"));
	

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
	BOOL bRet = Init();
	bRet |= InitUI();
	return bRet;// return TRUE  unless you set the focus to a control
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

void CMiniBlogDlg::AutoClickAD()
{
	if(!m_pCMSvr)
		return;
	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_GET_AD;

	SetClintIDForTask(&tp);
	m_pCMSvr->AddTask(&tp);

}

void CMiniBlogDlg::OnBnClickedButton1Test()
{

	TCHAR szHEX[50]={0};
	CM_Encrypt(szHEX,_T("93732717"));
	TCHAR szBin[25] = {0};
	CM_Decrypt(szBin,(PBYTE)"E12C309D0038C3D9");
	//SetTimer(TIMER_AUTO_START,1000*60,NULL);
	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_GET_AD;
	
	//SetClintIDForTask(&tp);
	//m_pADsvr->AddTask(&tp);

	tp.dwTaskType = ACT_SINA_UPLOAD_HEADIMG;
	m_pSinaSvr->AddTask(&tp);

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
	AddFansToGrid(strName,NULL);
	EnableAddUser(FALSE);

	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_LOGIN_SINA;
	_tcscpy(tp.user.szUserName,strName.GetBuffer());
	_tcscpy(tp.user.szUserPwd,strPwd.GetBuffer());

	m_pSinaSvr->AddTask(&tp);

	// TODO: Add your control notification handler code here
}
void CMiniBlogDlg::ResetUerWND()
{
	m_edtUsername.SetWindowText(_T(""));
	m_edtUserPwd.SetWindowText(_T(""));

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
void CMiniBlogDlg::AddFansToGrid(LPCTSTR lpName,LPCTSTR lpStatus)
{
	int nRow = m_Grid.GetRowCount();
	m_Grid.InsertRow(_T("test"));
	GV_ITEM Item;
	CString str;
	USERINFO uinfo;
	_tcscpy(uinfo.szName,lpName);
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
			if (NULL == lpStatus)
			{
			Item.strText = _T("登录中...");
			}
			else
				Item.strText = lpStatus;
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
	case TIMER_AUTO_START_AD:
		AutoClickAD();
		break;
	case TIMER_DELAY_CHECK_VER:
		{
			KillTimer(TIMER_DELAY_CHECK_VER);
			BOOL bNeedUpdate = CheckForUpdate();
			if (bNeedUpdate)
			{
				int nRet = AfxMessageBox(_T("检测到有新版本，是否要升级？"),MB_YESNO);
				if (nRet == IDYES)
				{
					CString csFile = GetModuleDirectory(AfxGetInstanceHandle())+_T("AutoUpdate.exe");
				//	::PostMessage(m_hWnd,WM_CLOSE,NULL,NULL);
					StartUpdate(csFile);

				}
			}
		}
		break;
	case TIMER_GET_ONLINE_CLIENT:
		getOnlineUsr();
		break;
	case TIMER_GET_CLIENT_LOGON:
		{
			KillTimer(TIMER_GET_CLIENT_LOGON);
			ClientLogon();
		}
		break;
	default:
		break;

	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMiniBlogDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	ClientLogout();
	KillTimer(TIMER_AUTO_START_AD);
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
	GetMAC(szMAC);
	TCHAR szMagic[100] = {0};
	_stprintf(szMagic,_T("www.centmind.com/www.weibojuntuan.com/%s"),szMAC);

	char cMagic[100] = {0};
	WideCharToMultiByte(CP_ACP,0,szMagic,-1,cMagic,100,NULL,NULL);
	char *cHash = GetMD5(cMagic);
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
void CMiniBlogDlg::ClientLogon()
{
	TCHAR szURL[MAX_PATH] = {0};
	_stprintf(szURL,_T("%s/?actid=%s&cid=%s&f=1"),SERVER_URL,TASK_ACT_ID_9,m_szClientID);
	PBYTE pBuf = HttpGet(szURL,TRUE);
	if (pBuf)
	{
		if (_tcslen(LPCWSTR(pBuf))<10)
		{
			TCHAR szCnt[MAX_PATH] = {0};
			_stprintf(szCnt,_T("在线用户：%s"),(LPCWSTR)pBuf);
			m_stcOnlineCnt.SetWindowText(szCnt);
		}
		else
			m_stcOnlineCnt.SetWindowText(_T("在线用户：未知"));

		delete [] pBuf;
	}

}
void CMiniBlogDlg::ClientLogout()
{
	TCHAR szURL[MAX_PATH] = {0};
	_stprintf(szURL,_T("%s/?actid=%s&cid=%s&f=0"),SERVER_URL,TASK_ACT_ID_9,m_szClientID);
	HttpGet(szURL,FALSE);
}
void CMiniBlogDlg::getOnlineUsr()
{
	TCHAR szURL[MAX_PATH] = {0};
	_stprintf(szURL,_T("%s/?actid=%s&cid=%s"),SERVER_URL,TASK_ACT_ID_10,m_szClientID);

	 PBYTE pBuf = HttpGet(szURL,TRUE);
	 if (pBuf)
	 {
		 if (_tcslen(LPCWSTR(pBuf))<10)
		 {
			 TCHAR szCnt[MAX_PATH] = {0};
			 _stprintf(szCnt,_T("在线用户：%s"),(LPCWSTR)pBuf);
			 m_stcOnlineCnt.SetWindowText(szCnt);
		 }
		 else
			 m_stcOnlineCnt.SetWindowText(_T("在线用户：未知"));

		 delete [] pBuf;
	 }
}
int CMiniBlogDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	TrayMessage(NIM_ADD);
	::SetProp(m_hWnd,   APP_NAME,   (HANDLE)1);  
	SetTimer(TIMER_DELAY_CHECK_VER,1000,NULL);
	SetTimer(TIMER_GET_ONLINE_CLIENT,5*60*1000,NULL);
	SetTimer(TIMER_GET_CLIENT_LOGON,50,NULL);
#ifdef _DEBUG
	SetTimer(TIMER_AUTO_START_AD,1000,NULL);
#else
	SetTimer(TIMER_AUTO_START_AD,60000,NULL);
#endif // _DEBUG
	
	

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
			case SINA_LOGIN_LOGGING:
				{
					m_Grid.SetItemText(i,2,_T("登录中..."));
				}
				break;
			default:
				{
					m_Grid.SetItemText(i,2,_T("登录失败，未知错误类型"));
				}
				break;
			}
			m_Grid.Refresh();
			return;
		}
	}
}
void CMiniBlogDlg::PostSInfo(USERINFO *pui)
{
	if (m_pCMSvr)
	{
		TASK_PARAM tp = {0};
		tp.dwTaskType = ACT_POST_S_INFO;
		_tcscpy(tp.user.szUID,pui->szUID);
		_tcscpy(tp.user.szUserName,pui->szName);
		_tcscpy(tp.user.szUserPwd,pui->szPWD);
		SetClintIDForTask(&tp);
		m_pCMSvr->AddTask(&tp);
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

	case  SINA_LOGIN_SUCCESS:
		{

			PostSInfo(&ui);
		}
		// no break here.
	case SINA_PWD_ERROR:
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
			EnableAddUser(TRUE);
		break;

	}
	ResetUerWND();
	if (nCode == SINA_LOGIN_SUCCESS && !m_bSmartLogon)
	{
		AddUserToDB(&ui);
	}
	if (m_bSmartLogon)
		LogonNext();
	return 0;
}
void CMiniBlogDlg::SetClintIDForTask(LPTASK_PARAM lptp)
{
	if (lptp)
	{
		_tcscpy(lptp->szClientID,m_szClientID);
	}
}
void CMiniBlogDlg::OnBnClickedButtonSmartLogon()
{
	m_bSmartLogon = TRUE;
	LogonNext();

	// TODO: Add your control notification handler code here
}


void CMiniBlogDlg::OnBnClickedButtonPostAd()
{
	// TODO: Add your control notification handler code here
	CString strURL ;
	m_edtADURL.GetWindowText(strURL);
	DWORD dwType = 0;
	CString strSevr,strObj;
	WORD wPort = 0;
	BOOL bRet = AfxParseURL(strURL.GetBuffer(),dwType,strSevr,strObj,wPort);
	if (bRet && dwType == AFX_INET_SERVICE_HTTP)
	{
		TASK_PARAM tp = {0};
		tp.dwTaskType = ACT_POST_AD;
		SetClintIDForTask(&tp);
		_tcscpy(tp.ad.szURL,strURL.GetBuffer());
		m_pCMSvr->AddTask(&tp);

#ifndef _DEBUG
		m_btnADPost.EnableWindow(FALSE);
		m_edtADURL.EnableWindow(FALSE);
#endif // _DEBUG
	}
	else
	{
		AfxMessageBox(_T("请输入正确的广告地址！"));
	}



}

void CMiniBlogDlg::OnEnKillfocusEditAd()
{

	// TODO: Add your control notification handler code here
}
void CMiniBlogDlg::LogonNext()
{
	if (!m_bSmartLogon)
		return;
	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_LOGIN_SINA;
	std::deque<USERINFO>::iterator beg = m_vtSmartLogonList.begin();
	std::deque<USERINFO>::iterator end = m_vtSmartLogonList.end();

	if (beg != end)
	{
		_tcscpy(tp.user.szUserName,beg->szName);
		_tcscpy(tp.user.szUserPwd,beg->szPWD);

		m_pSinaSvr->AddTask(&tp);
		USERINFO ui = m_vtSmartLogonList.at(0);
		SetUserStatus(&ui,SINA_LOGIN_LOGGING);
		m_vtSmartLogonList.pop_front();
	}
	else
		m_bSmartLogon = FALSE;
	
}

BOOL CMiniBlogDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	m_ToolTip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}
