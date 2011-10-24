// AutoUpdateDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define WM_POSMESSAGE WM_USER+100
#define WM_ENDMESSAGE WM_USER+101

// CAutoUpdateDlg 对话框
class CAutoUpdateDlg : public CDialog
{
// 构造
public:
	CAutoUpdateDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
	CString m_csDownloadURL;
	BOOL    m_bFileOK;
private:
	DWORD m_dwDownloadedSize;
	DWORD m_dwTotalSize;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnPositionMessage(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnEndMessage(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctlProcess;
	CStatic m_ctPercentValue;
	afx_msg void OnBnClickedCancel();

public:
	static DWORD WINAPI DoRun(LPVOID lParam);
private:
	DWORD DoDownLoadThread(void);
	BOOL DownloadFile(LPCSTR lpURL, LPCSTR lpFileXml);
};
