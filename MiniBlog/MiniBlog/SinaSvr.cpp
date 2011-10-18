#include "StdAfx.h"
#include "SinaSvr.h"

CSinaSvr::CSinaSvr(HWND hWnd):
m_hWnd(hWnd)
{
	ZeroMemory(&m_uiCurUserInfo,sizeof(m_uiCurUserInfo));
}


CSinaSvr::~CSinaSvr(void)
{
}
BOOL CSinaSvr::CreateFromControl(CWnd *pParentWnd,UINT nID)
{
	BOOL result = FALSE;

	ASSERT(pParentWnd != NULL);
	OpenWebBrowser();
	CRect		rect(0,0,0,0);
	
	CWnd *control = pParentWnd->GetDlgItem(nID);
	/*
	if (control != NULL) {
		control->GetWindowRect(&rect);
		pParentWnd->ClientToScreen(&rect);
		rect.left -= 5;
		rect.right -= 5;
		rect.top -= 100;
		rect.bottom = rect.top + 150;
		// destroy control, since the browser will take its place
		control->DestroyWindow();

	}*/
	SetWebRect(&rect);

	return result;
}

void CSinaSvr::OnDocumentCompleted(LPCTSTR lpURL)
{
	if (GetAction() == ACT_LOGIN_SINA)
	{
		SetActionStatus(SINA_LOGINING);
		CheckLoginStatus(lpURL);

	}
}

HRESULT CSinaSvr::GetExternal(IDispatch **ppDispatch)
{
	//重写GetExternal返回一个ClientCall对象
	return S_OK;
}

HRESULT CSinaSvr::AddTask(LPTASK_PARAM lpTaskParam)
{
	if (lpTaskParam == NULL)
	{
		return E_FAIL;
	}
	SetAction((TASK_ACTION)lpTaskParam->dwTaskType);
	switch(lpTaskParam->dwTaskType)
	{
	case ACT_LOGIN_SINA:
		{
			ZeroMemory(&m_uiCurUserInfo,sizeof(m_uiCurUserInfo));
			Login(lpTaskParam->user.szUserName,lpTaskParam->user.szUserPwd);
		}
		break;
	case ACT_POST_SINA:
		{
			PostWeibo(lpTaskParam->post.szContent);
		}
		break;
	case ACT_FOLLOW_SINA:
		{
			Follow(lpTaskParam->post.szUID);
		}
		break;
	case ACT_COMMENT_SINA:
		{
			Comment(lpTaskParam->post.szBlogID,lpTaskParam->post.szUID,lpTaskParam->post.szContent);
		}
		break;
	case ACT_FORWARD_SINA:
		{
			Forward(lpTaskParam->post.szBlogID,lpTaskParam->post.szUID,lpTaskParam->post.szContent);
		}
		break;

	}

	return S_FALSE;
}

void CSinaSvr::Login(LPCTSTR lpUserName,LPCTSTR lpPwd)
{

	SetAction(ACT_LOGIN_SINA);
	TCHAR szPost[1024]= {0};
	_stprintf(szPost,_T("username=%s&password=%s&entry=miniblog&act=1&from=referer%3Awww_index"),lpUserName,lpPwd);
	_stprintf(m_uiCurUserInfo.szName,_T("%s"),lpUserName);
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
void CSinaSvr::Follow(LPCTSTR  lpUID)
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
#if 1
void CSinaSvr::PostWeibo(LPCTSTR lpContent)
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
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest"),m_uiCurUserInfo.szUID);

	TCHAR szURL[1024]=_T("http://weibo.com/aj/mblog/add");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	//Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);
}
#else
void CSinaSvr::PostWeibo(LPCTSTR lpContent)
{
	BOOL bResults= FALSE;
HINTERNET  hSession = NULL,hConnect = NULL,	hRequest = NULL;
	TCHAR szServer[1024] = _T("weibo.com");
	TCHAR szParam[1024]  = _T("aj/mblog/add");
	TCHAR szHeader[1024] = {0};
	TCHAR szCookie[1024] = {0};
	_stprintf(szHeader,_T("Accept: */*\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://weibo.com/%s\r\nx-requested-with: XMLHttpRequest\r\n"),m_szCurUID);
	_stprintf(szCookie,_T("Cookie: %s\r\n"),m_strCookie.GetBuffer());
	WCHAR szwPost[1024]={0};
	_stprintf(szwPost,_T("_t=0&pic_id=&text=%s"),lpContent);

	char szPost[1024]= {0};
	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);

	PBYTE pBuf = new BYTE[1024*1024];

	hSession = WinHttpOpen(0,  
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME, 
		WINHTTP_NO_PROXY_BYPASS, 0 );
	
	if(hSession)
		WinHttpSetTimeouts( hSession, 600000, 600000, 600000, 600000);

	if( hSession )
		hConnect = WinHttpConnect( hSession,szServer,80, 0 );

	if (hConnect)
        hRequest = WinHttpOpenRequest( hConnect, L"POST", szParam, 
		NULL, WINHTTP_NO_REFERER, 
		WINHTTP_DEFAULT_ACCEPT_TYPES, 
		0);
	int nLen = strlen(szPost);
	WinHttpAddRequestHeaders( hRequest, 
		szCookie, 
		-1, 
		WINHTTP_ADDREQ_FLAG_ADD);
	// Send a Request.
    if (hRequest) 
		bResults = WinHttpSendRequest( hRequest, szHeader,-1, szPost, nLen,nLen,0);

	DWORD dwLen;
	GetHttpResponse(pBuf,dwLen,hRequest);

	if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
	
	delete []pBuf;
	PostWeibo1(lpContent);
}
#endif

void CSinaSvr::Forward(LPCTSTR  lpMid,LPCTSTR lpUid,LPCTSTR lpReason)
{
	
	TCHAR szMid[MAX_PATH] = _T("3366217727338022");
	_tcscpy(szMid,lpMid);
	TCHAR szUid[] = _T("2400232192");
	WCHAR szwPost[1024]={0};
	TCHAR szReason[] = _T("转发微博！");
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

void CSinaSvr::Comment(LPCTSTR lpMid,LPCTSTR lpUID,LPCTSTR lpContent)
{
	TCHAR szMid[MAX_PATH] = _T("3366217727338022");
	TCHAR szUid[] = _T("2400232192");
	WCHAR szwPost[1024]={0};
	TCHAR szContent[1024] = _T("comment-test");
	
	_tcscpy(szMid,lpMid);
	_tcscpy(szUid,lpUID);
	_tcscpy(szContent,lpContent);
	
	_stprintf(szwPost,_T("act=post&mid=%s&uid=%s&forward=0&isroot=1&content=%s&_t=0"),szMid,m_uiCurUserInfo.szUID,szContent);
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

	TCHAR szURL[1024]=_T("http://weibo.com/aj/comment/add");
	COleVariant vPostData = arr;
	COleVariant vURL(szURL, VT_BSTR);
	COleVariant vHeaders(szHeader, VT_BSTR);
	COleVariant vTargetFrameName((LPCTSTR)NULL, VT_BSTR);
	COleVariant vFlags((long) NULL, VT_I4);
	Navigate2(vURL, vFlags, vTargetFrameName,vPostData, vHeaders);

}
HRESULT CSinaSvr::CheckLoginStatus(CString URL)
{
	if(m_ActionStatus != SINA_LOGINING)
		return S_FALSE;

	/*
	check login pwd
	//http://login.sina.com.cn/sso/login.php
	*/
	long ulen = 0;
	CComQIPtr<IHTMLDocument3> pDoc3 = GetDocument3();
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
			SetActionStatus(SINA_PWD_ERROR);
			SetAction(ACT_NULL);
			PostMessage(m_hWnd,WM_USER_LOGIN_STATUS,(WPARAM)&m_uiCurUserInfo,SINA_PWD_ERROR);
			return S_OK;
			
		}
		return S_OK;
	}
	


	//http://weibo.com/signup/full_info.php?uid=2452258262&type=2&r=/2452258262 not regist for weibo.
	if (URL.Find(_T("http://weibo.com/signup/full_info.php?uid="),0) >= 0)
	{
		SetActionStatus(SINA_NO_WEIBO);
		SetAction(ACT_NULL);
		PostMessage(m_hWnd,WM_USER_LOGIN_STATUS,(WPARAM)&m_uiCurUserInfo,SINA_NO_WEIBO);
		return S_OK;
	}

	// login ok
	if (URL.Find(_T("http://weibo.com/"),0) >= 0)
	{
		SetActionStatus(SINA_LOGIN_SUCCESS);
		SetAction(ACT_NULL);
		GetUID();
		PostMessage(m_hWnd,WM_USER_LOGIN_STATUS,(WPARAM)&m_uiCurUserInfo,SINA_LOGIN_SUCCESS);
		return S_OK;
	}
	PostMessage(m_hWnd,WM_USER_LOGIN_STATUS,(WPARAM)&m_uiCurUserInfo,SINA_NONE);

	
#if 0


	CString strHtml;
	GetCookie(m_strCookie);
	GetHtml(strHtml);
	CString strSrcipt;
	GetScript(strSrcipt);
	FILE *f = _tfopen(_T("cookie.txt"),_T("w"));
	fwrite(m_strCookie.GetBuffer(),1,m_strCookie.GetLength()*2,f);
	fclose(f);
	
	f = _tfopen(_T("html.txt"),_T("w"));
	int len = strHtml.GetLength();
	char * p = new char[len];
	WideCharToMultiByte(CP_ACP,0,strHtml.GetBuffer(),-1,p,len,0,0);
// 	char szPost[1024]= {0};
// 	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);

	fwrite(p,1,len,f);
	fclose(f);
	delete [] p;


	f = _tfopen(_T("script.txt"),_T("w"));
	len = strSrcipt.GetLength();
	p = new char[len];
	WideCharToMultiByte(CP_ACP,0,strSrcipt.GetBuffer(),-1,p,len,0,0);
	// 	char szPost[1024]= {0};
	// 	WideCharToMultiByte(CP_UTF8, 0, szwPost, -1, szPost, MAX_PATH, NULL, NULL);

	fwrite(p,1,len,f);
	fclose(f);
	delete [] p;
#endif

	return S_OK;
}

HRESULT CSinaSvr::GetUID()
{
	CComQIPtr<IHTMLDocument2> pDoc2 = GetDocument2();
	CComQIPtr<IHTMLElementCollection> i_Collect;
	HRESULT hr = pDoc2->get_scripts(&i_Collect);
	if (!i_Collect)
		return FALSE;
	long uLen = 0;
	i_Collect->get_length(&uLen);
	BSTR   ElementSrcText; 
	for (long i=0;i<uLen;i++){
		CComQIPtr<IHTMLScriptElement> iScript;
		CComQIPtr<IDispatch> i_Dispath;
		i_Dispath   =   getElementInCollection(i_Collect,i);
		i_Dispath-> QueryInterface(IID_IHTMLScriptElement,(void   **)&iScript); 

		//iScript-> get_src(&ElementSrcText); 
		iScript->get_text(&ElementSrcText);
		CString str = ElementSrcText;
		int nPos = str.Find(_T("$CONFIG['uid']"),0);
		if (nPos >= 0)
		{
			int nPos1 = str.Find(_T(";"),nPos);
			//$CONFIG['uid'] = '1828546224'
			str = str.Mid(nPos,nPos1-nPos);
			nPos = str.Find(_T("="),0);
			if ( nPos > 0 )
			{
				nPos = str.Find(_T("'"),nPos);
				nPos1 = str.Find(_T("'"),nPos+1);
				str = str.Mid(nPos,nPos1-nPos);
				_stprintf(m_uiCurUserInfo.szUID,_T("%s"),str.GetBuffer());
				//_tcscpy(m_szCurUID,str.GetBuffer());
			}
			break;
		}

	}
	return TRUE;

}