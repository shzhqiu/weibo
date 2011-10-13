
// MiniBlogDlg.h : header file
//
#pragma once
#include "SinaBrowserTool.h"
#include "SQLiteTool.h"
#include "afxwin.h"
#include "GridCtrl/GridCtrl.h"
#include "GridCtrl/GridURLCell.h"
#include "GridCtrl/GridCellCombo.h"
#include "GridCtrl/GridCellCheck.h"
#include "GridCtrl/GridCellNumeric.h"
#include "GridCtrl/GridCellDateTime.h"
#include "TaskMgr.h"


// CMiniBlogDlg dialog
class CMiniBlogDlg : public CDialogEx
{
// Construction
public:
	CMiniBlogDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MINIBLOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void GetTask();

public:
	afx_msg void OnBnClickedButton1Test();
	afx_msg void OnBnClickedButtonAddUser();
	
private:
	CEdit m_Username;
	CEdit m_UserPwd;
private:
	BOOL Init();
	BOOL InitUI();
	BOOL InitGrid();
	void AddFansToGrid(LPCTSTR lpName,LPCTSTR lpPWD);
private:
	SinaBrowserTool *m_pSinaSvr;
	CTaskMgr     *m_pTaskMgr;
	CSQLiteTool *m_pDB;
	CGridCtrl  m_Grid;
	TASK_PARAM  m_TaskParam;


public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
