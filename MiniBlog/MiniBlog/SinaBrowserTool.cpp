#include "StdAfx.h"
#include "SinaBrowserTool.h"


SinaBrowserTool::SinaBrowserTool(void)
{
	m_CurTask = ACT_END;

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
HRESULT SinaBrowserTool::CheckLoginStatus()
{
	/*
	IHTMLDocument2 *doc2 = GetDocument();

	IPersistFile   *pf   =   NULL; 
	//   获取了IHTMLDocument2接口以后 

	//   保存网页文档 
	if(SUCCEEDED(doc2-> QueryInterface(IID_IPersistFile,   (void   **)&pf))) 
	{ 
		pf->GetCurFile()
	} 
		/*
		CComQIPtr<IHTMLElementCollection> i_Collect;
		long u32_Len;

		IHTMLDocument3 *doc3 = GetDocument3();
		
		IHTMLDocument2 *doc2 = GetDocument();
		doc2->get_scripts(&i_Collect);
		i_Collect->get_length(&u32_Len);
		CString       s_TagName=_T("retcode"); // IN
		CComQIPtr<IHTMLElementCollection> i_Collect;
		CComBSTR bs_TagName = s_TagName;
		doc3->getElementsByTagName(bs_TagName, &i_Collect);
		i_Collect->get_length(&u32_Len);
		u32_Len ++;
		*/

	return S_OK;
}
void SinaBrowserTool::OnDocumentComplete(CString URL)
{
	SimpleBrowser::OnDocumentComplete(URL);

	if (GetAction() == ACT_LOGIN_SINA)
	{
		CheckLoginStatus();
		
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
// no use now
void SinaBrowserTool::AddTask(MINIBLOG_ACTION task)
{
	return ;
}
void SinaBrowserTool::Login(LPCTSTR lpUserName,LPCTSTR lpPwd)
{

	SetAction(ACT_LOGIN_SINA);
	TCHAR szPost[1024]= {0};
	_stprintf(szPost,_T("username=%s&password=%s&entry=miniblog&act=1&from=referer%3Awww_index"),lpUserName,lpPwd);

	//int iSize = strlen(szPost);

	CByteArray arr;
	for (int i = 0; i < 1024; i++)
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