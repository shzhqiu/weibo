
// MiniBlogDlg.h : header file
//
#pragma once
#include "afxwin.h"
#include "GridCtrl/GridCtrl.h"
#include "GridCtrl/GridURLCell.h"
#include "GridCtrl/GridCellCombo.h"
#include "GridCtrl/GridCellCheck.h"
#include "GridCtrl/GridCellNumeric.h"
#include "GridCtrl/GridCellDateTime.h"
#include "TaskMgr.h"
#include "SinaSvr.h"
#include "ADTask.h"
#include "SQLiteBase.h"
#include "SinaSQLTool.h"

#define  APP_NAME _T("WEIBOJUNTUAN_CENTMIND_COM")
#define  TIMER_AUTO_START       (WM_USER+2011)
#define  MYWM_NOTIFYICON		(WM_USER+2012)
#define  TIMER_DELAY_CHECK_VER   (WM_USER+2013)
#define  WM_USER_LOGIN_STATUS    (WM_USER+2020)



// CMiniBlogDlg dialog
class CMiniBlogDlg : public CDialogEx
{
// Construction
public:
	CMiniBlogDlg(CWnd* pParent = NULL);	// standard constructor
	~CMiniBlogDlg();

// Dialog Data
	enum { IDD = IDD_MINIBLOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLoginStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void GetTask();

public:
	afx_msg void OnBnClickedButton1Test();
	afx_msg void OnBnClickedButtonAddUser();
	
private:
	CEdit m_edtUsername;
	CEdit m_edtUserPwd;
private:
	BOOL Init();
	BOOL InitUI();
	BOOL InitGrid();
	void AddFansToGrid(LPCTSTR lpName,LPCTSTR lpStatus);
	void AddUserToDB(LPUSERINFO lpUI);
	void InitClientID();
	BOOL TrayMessage( DWORD dwMessage);
	void LoadDB();

	void SetClintIDForTask(LPTASK_PARAM lptp);
	
	
	BOOL IsUserAdded(LPCTSTR lpUserName);
	void EnableAddUser(BOOL bEnable);
	void SetUserStatus(USERINFO *pui,int nStatus);

private:
	CSinaSvr	 *m_pSinaSvr;
	CADTask		 *m_pADsvr;
	CTaskMgr     *m_pTaskMgr;
	CSQLiteBase *m_pDB;
	CSinaSQLTool *m_pSinaSQL;
	CGridCtrl	m_Grid;
	TASK_PARAM  m_TaskParam;
	TCHAR		m_szClientID[33];//MD5:32BITS
	std::vector<USERINFO> m_vtUserList;
	USERINFO  m_CurUser;



public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	CButton m_btnAddUser;
	afx_msg void OnBnClickedButtonSmartLogon();
};
