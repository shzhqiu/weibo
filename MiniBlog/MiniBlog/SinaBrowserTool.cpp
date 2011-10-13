#include "StdAfx.h"
#include "SinaBrowserTool.h"
#include <regex>

SinaBrowserTool::SinaBrowserTool(void)
{
	m_CurTask = ACT_END;
	m_ActionStatus = SINA_NONE;
	m_strCurURL = _T("");
	_tcscpy(m_szCurUID,_T("1828546224"));
}


SinaBrowserTool::~SinaBrowserTool(void)
{
}


bool SinaBrowserTool::OnBeforeNavigate2(CString URL,
	CString frame,
	void    *post_data,int post_data_size,
	CString headers)
{
	SimpleBrowser::OnBeforeNavigate2(URL,frame,post_data,post_data_size,headers);

	CString event;
	event.Format(_T("OnBeforeNavigation: URL=\"%s\", frame=\"%s\", post_data=[0x%08X,%d bytes], headers=\"%s\""),
		URL,frame,post_data,post_data_size,headers);

	return false;
}


HRESULT SinaBrowserTool::CheckLoginStatus(CString URL)
{
	if(m_ActionStatus != SINA_LOGINING)
		return S_FALSE;

	/*
	check login pwd
	//http://login.sina.com.cn/sso/login.php
	*/
	long ulen = 0;
	IHTMLDocument3 *pDoc3 = GetDocument3();
	HRESULT hr = E_FAIL;
	if (URL.Find(_T("http://login.sina.com.cn/sso/login.php"),0) >= 0)
	{
		CComBSTR strName = _T("retcode");
		CComQIPtr<IHTMLElementCollection> i_Collect;
		CComQIPtr<IDispatch> i_Dispath;
		CComQIPtr<IHTMLInputElement> iInput;
		pDoc3->getElementsByName(strName,&i_Collect);
		i_Collect->get_length(&ulen);
		for (long i=0;i<ulen;i++){
			i_Dispath   =   getElementInCollection(i_Collect,i);
			hr = i_Dispath-> QueryInterface(IID_IHTMLInputElement,(void   **)&iInput); 
			CComBSTR str;
			iInput->get_value(&str);
			m_ActionStatus = SINA_PWD_ERROR;
			return S_OK;
			
		}
	}

	//http://weibo.com/signup/full_info.php?uid=2452258262&type=2&r=/2452258262 not regist for weibo.
	if (URL.Find(_T("http://weibo.com/signup/full_info.php?uid="),0) >= 0)
	{
		m_ActionStatus = SINA_NO_WEIBO;
		return S_OK;
	}
	
	pDoc3->Release();
	
	return S_OK;
}
void SinaBrowserTool::OnDocumentComplete(CString URL)
{
	SimpleBrowser::OnDocumentComplete(URL);

	if (GetAction() == ACT_LOGIN_SINA)
	{
		m_ActionStatus = SINA_LOGINING;
		CheckLoginStatus(URL);
		
	}

}

void SinaBrowserTool::OnDownloadBegin()
{
	SimpleBrowser::OnDownloadBegin();

}

void SinaBrowserTool::OnProgressChange(int progress,int progress_max)
{
	SimpleBrowser::OnProgressChange(progress,progress_max);

	CString event;
	event.Format(_T("OnProgressChange: progress=%d, progress_max=%d"),
		progress,progress_max);

}

void SinaBrowserTool::OnDownloadComplete()
{
	SimpleBrowser::OnDownloadComplete();


}
void SinaBrowserTool::OnNavigateComplete2(CString URL)
{
	SimpleBrowser::OnNavigateComplete2(URL);
	m_strCurURL = URL;
	CString event;
	event.Format(_T("OnNavigationComplete2: URL=\"%s\""),URL);

}

void SinaBrowserTool::OnStatusTextChange(CString text)
{
	SimpleBrowser::OnStatusTextChange(text);

	CString event;
	event.Format(_T("OnStatusTextChange: text=\"%s\""),text);

}

void SinaBrowserTool::OnTitleChange(CString text)
{
	SimpleBrowser::OnTitleChange(text);

	CString event;
	event.Format(_T("OnTitleChange: text=\"%s\""),text);

}

void SinaBrowserTool::Login(LPCTSTR lpUserName,LPCTSTR lpPwd)
{

	SetAction(ACT_LOGIN_SINA);
	TCHAR szPost[1024]= {0};
	_stprintf(szPost,_T("username=%s&password=%s&entry=miniblog&act=1&from=referer%3Awww_index"),lpUserName,lpPwd);

	int iSize = _tcslen(szPost);

	CByteArray arr;
	for (int i = 0; i < iSize; i++)
	{
		arr.Add(szPost[i]);
	}

	TCHAR szHeader[1024] = _T("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");

	TCHAR szURL[1024]=_T("http://login.sina.com.cn/sso/login.php");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);

}
void SinaBrowserTool::Follow(LPCTSTR  lpUID)
{
	TCHAR szUid[20] = _T("2436235444");
	//_tcscpy(szUid,lpUid);
	TCHAR szPost[1024]= {0};
	_stprintf(szPost,_T("uid=%s&f=1&extra=refer_sort%3Apl_content_hisPersonalInfo&_t=0"),szUid);
	int iSize = _tcslen(szPost);
	
	CByteArray arr;
	for (int i = 0; i < iSize; i++)
	{
		arr.Add(szPost[i]);
	}

	TCHAR szHeader[1024] = {0};
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest"),szUid);

	TCHAR szURL[1024]=_T("http://weibo.com/aj/f/followed");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);

}
void SinaBrowserTool::PostWeibo(LPCTSTR lpContent)
{

// 	//check URL;
// 	if (m_strCurURL.Find(_T("http://weibo.com/",0)) < 0)
// 		return;
	WCHAR szwPost[1024]={0};
	_stprintf(szwPost,_T("_t=0&pic_id=&text=%s"),lpContent);

	char szPost[1024]= {0};
	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);
	
	int iSize = strlen(szPost);
	
	CByteArray arr;
	for (int i = 0; i < iSize; i++)
	{
		arr.Add(szPost[i]);
	}

	TCHAR szHeader[1024] = {0};
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest"),m_szCurUID);

	TCHAR szURL[1024]=_T("http://weibo.com/aj/mblog/add");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);
}
void SinaBrowserTool::Forward(LPCTSTR  lpMid,LPCTSTR lpUid,LPCTSTR lpReason)
{
	
	TCHAR szMid[MAX_PATH] = _T("3366217727338022");
	_tcscpy(szMid,lpMid);
	TCHAR szUid[] = _T("2400232192");
	WCHAR szwPost[1024]={0};
	TCHAR szReason[] = _T("×ª·¢Î¢²©£¡");
	_tcscpy(szReason,lpReason);
	_stprintf(szwPost,_T("appkey=&mid=%s&style_type=1&reason=%s&_t=0"),szMid,szReason);

	char szPost[1024]= {0};
	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);
	
	int iSize = strlen(szPost);
	
	CByteArray arr;
	for (int i = 0; i < iSize; i++)
	{
		arr.Add(szPost[i]);
	}

	TCHAR szHeader[1024] = {0};
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest"),szUid);

	TCHAR szURL[1024]=_T("http://weibo.com/aj/mblog/forward");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);

}

void SinaBrowserTool::Comment(LPCTSTR lpMid,LPCTSTR lpUID,LPCTSTR lpContent)
{
	TCHAR szMid[MAX_PATH] = _T("3366217727338022");
	TCHAR szUid[] = _T("2400232192");
	WCHAR szwPost[1024]={0};
	TCHAR szContent[1024] = _T("comment-test");
	
	_tcscpy(szMid,lpMid);
	_tcscpy(szUid,lpUID);
	_tcscpy(szContent,lpContent);
	
	_stprintf(szwPost,_T("act=post&mid=%s&uid=%s&forward=0&isroot=1&content=%s&_t=0"),szMid,m_szCurUID,szContent);
	//_stprintf(szwPost,_T("act=post&mid=3366217727338022&uid=1828546224&forward=0&isroot=0&content=comment-test&type=big&_t=0"));
	char szPost[1024]= {0};
	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);
	
	int iSize = strlen(szPost);
	
	CByteArray arr;
	for (int i = 0; i < iSize; i++)
	{
		arr.Add(szPost[i]);
	}

	TCHAR szHeader[1024] = {0};
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest"),szUid);
	//_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nx-requested-with: XMLHttpRequest"),szUid);

	TCHAR szURL[1024]=_T("http://weibo.com/aj/comment/add");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);

}
HRESULT SinaBrowserTool::ProcessTask(LPTASK_PARAM lpTaskParam)
{
	if (lpTaskParam == NULL)
	{
		return E_FAIL;
	}
	switch(lpTaskParam->dwTaskType)
	{
	case ACT_POST_SINA:
		PostWeibo(lpTaskParam->szContent);
		break;
	}
	return S_FALSE;
}
