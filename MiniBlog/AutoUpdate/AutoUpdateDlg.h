// AutoUpdateDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define WM_POSMESSAGE WM_USER+100
#define WM_ENDMESSAGE WM_USER+101

// CAutoUpdateDlg �Ի���
class CAutoUpdateDlg : public CDialog
{
// ����
public:
	CAutoUpdateDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
	CString m_csDownloadURL;
	BOOL    m_bFileOK;
private:
	DWORD m_dwDownloadedSize;
	DWORD m_dwTotalSize;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
