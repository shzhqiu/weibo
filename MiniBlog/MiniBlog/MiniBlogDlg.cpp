
// MiniBlogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiniBlog.h"
#include "MiniBlogDlg.h"
#include "afxdialogex.h"
#include "SinaSvr.h"

#define  TIMER_AUTO_START WM_USER+2011
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
	m_pDB = new CSQLiteTool();
	m_pTaskMgr = new CTaskMgr();
}
CMiniBlogDlg::~CMiniBlogDlg()
{
	delete m_pTaskMgr;
	delete m_pDB;
	delete m_pSinaSvr;

};
void CMiniBlogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_Username);
	DDX_Control(pDX, IDC_EDIT_USERPWD, m_UserPwd);
	DDX_Control(pDX, IDC_GRID_CTRL, m_Grid);
}

BEGIN_MESSAGE_MAP(CMiniBlogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1_TEST, &CMiniBlogDlg::OnBnClickedButton1Test)
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CMiniBlogDlg::OnBnClickedButtonAddUser)
	ON_WM_TIMER()
	ON_WM_DESTROY()
//	ON_WM_CREATE()
//ON_WM_CREATE()
ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CMiniBlogDlg::Init()
{
	if(!m_pDB)
		return FALSE;
	m_pDB->OpenDB(NULL);
	return S_OK;
}

BOOL CMiniBlogDlg::InitGrid() 
{
	UpdateData();

	m_Grid.SetVirtualMode(FALSE);
	m_Grid.SetEditable(FALSE);
	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_Grid.SetFixedColumnSelection(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.EnableColumnHide();
	m_Grid.AutoSize();
	m_Grid.SetEditable(TRUE);
	m_Grid.SetHeaderSort(FALSE);
	 m_Grid.EnableTitleTips(TRUE);
/*	m_Grid.SetGridLines(GVL_NONE);
	m_Grid.SetGridLines(GVL_HORZ);
	m_Grid.SetGridLines(GVL_VERT);
	*/
	m_Grid.SetGridLines(GVL_BOTH);
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);


	
	{
		int nFixCols = 1;
		int nFixRows = 1;
		int nCols = GRID_ROW_COUNT;
		int nRows = 1;


		m_Grid.SetAutoSizeStyle();

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
						Item.strText = _T("ÐòºÅ");
						break;
					case 1:
						Item.strText = _T("ÕÊºÅ");
						break;
					case 2:
						Item.strText = _T("×´Ì¬");
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

	//m_Grid.GetDefaultCell(FALSE,FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
	//m_Grid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);

	// Having weird Autosize problems with CE 2.11 - possibly just an emulation problem
	m_Grid.AutoSize();


	UpdateData(FALSE);
	return TRUE;
}
BOOL CMiniBlogDlg::InitUI()
{
	if (!m_pSinaSvr)
		return FALSE;
	m_pSinaSvr = new CSinaSvr(GetSafeHwnd());
	m_pTaskMgr->SetSvr(m_pSinaSvr);
	m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
	InitGrid();
	//m_pSinaSvr->CreateFromControl(this,IDC_STATIC_BROWSER);
#ifdef _DEBUG
	//CString strName = _T("fortestonlya@sina.com");
	//CString strPwd = _T("fortestonlya");
	m_Username.SetWindowText(_T("shzhqiu@hotmail.com"));
	m_UserPwd.SetWindowText(_T("93732717"));


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

	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_LOGIN_SINA;
	_tcscpy(tp.user.szUserName,_T("shzhqiu@hotmail.com"));
	_tcscpy(tp.user.szUserPwd,_T("93732717"));

	m_pSinaSvr->ProcessTask(&tp);

	// TODO: Add your control notification handler code here
}


void CMiniBlogDlg::OnBnClickedButtonAddUser()
{
	
	CString strName;
	m_Username.GetWindowText(strName);
	CString strPwd;
	m_UserPwd.GetWindowText(strPwd);
	if (strName.GetLength() <10 || strPwd.GetLength() <= 5)
	{
		AfxMessageBox(L"ÇëÕýÈ·ÊäÈëÕÊºÅºÍÃÜÂë",MB_ICONINFORMATION);
		return;
	}

	AddFansToGrid(strName,strPwd);

	TASK_PARAM tp = {0};
	tp.dwTaskType = ACT_LOGIN_SINA;
	_tcscpy(tp.user.szUserName,strName.GetBuffer());
	_tcscpy(tp.user.szUserPwd,strPwd.GetBuffer());

	m_pSinaSvr->ProcessTask(&tp);

	// TODO: Add your control notification handler code here
}
void CMiniBlogDlg::AddFansToGrid(LPCTSTR lpName,LPCTSTR lpPWD)
{
	int nRow = m_Grid.GetRowCount();
	m_Grid.InsertRow(_T("test"));
	GV_ITEM Item;
	CString str;

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
			Item.strText = _T("µÇÂ¼ÖÐ...");
			m_Grid.SetItemState(nRow,i, m_Grid.GetItemState(nRow,i) & ~GVIS_READONLY);
			m_Grid.Invalidate();
			m_Grid.SetCellType(nRow,i, RUNTIME_CLASS(CGridCellCheck));
			break;
		}
		COLORREF clr = RGB(rand()%128 + 128, rand()%128 + 128, rand()%128 + 128);
		Item.crBkClr = clr;             // or - m_Grid.SetItemBkColour(row, col, clr);
		Item.crFgClr = RGB(255,0,0);    // or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		m_Grid.SetItem(&Item);

	}
	m_Grid.AutoSize();

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
	// TODO: Add your message handler code here
}


void CMiniBlogDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	RECT rc={0,0,0,0};
	//m_pSinaSvr->SetWebRect(&rc);
	// TODO: Add your message handler code here
}
