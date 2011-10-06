
// MiniBlogDlg.h : header file
//
#pragma once
#include "SinaBrowserTool.h"
#include "SQLiteTool.h"
#include "afxwin.h"
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
	afx_msg void OnBnClickedButton1Test();
	afx_msg void OnBnClickedButtonAddUser();
	
private:
	CEdit m_Username;
	CEdit m_UserPwd;
private:
	BOOL Init();
	BOOL InitUI();
private:
	SinaBrowserTool *m_pSinaSvr;
	CSQLiteTool *m_pDB;

};
