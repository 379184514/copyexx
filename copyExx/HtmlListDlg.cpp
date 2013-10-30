// HtmlListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "copyExx.h"
#include "HtmlListDlg.h"

ObjHtml objHtml[]=
{
	{	IDSORTUP,		_T("SortUp"),		_T("<span id=SortUp class=btn title=升序>△</span>")			},
	{	IDSORTDOWN,		_T("SortDown"),		_T("<span id=SortDown class=btn title=降序>▽</span>")			},
	{	IDSORTUPCUR,	_T("SortUpCur"),	_T("<span id=SortUpCur class=btn title=当前升序>▲</span>")		},
	{	IDSORTDOWNCUR,	_T("SortDownCur"),	_T("<span id=SortDownCur class=btn title=当前降序>▼</span>")	},
	{	IDUNFOLD,		_T("unfold"),		_T("<span id=unfold class=btn title=展开>[+]</span>")			},
	{	IDFOLD,			_T("fold"),			_T("<span id=fold  class=btn title=全部折叠>[ - ]</span>")		},
	{	IDTABHEAD,		_T("tabhead"),		_T("<THEAD id=tabhead><TR><TD>序号</TD>……</TR><THEAD>")		}
};

// CHtmlListDlg 对话框

IMPLEMENT_DYNCREATE(CHtmlListDlg, CDHtmlDialog)

CHtmlListDlg::CHtmlListDlg(CWnd* pParent /*=NULL*/)
: CDHtmlDialog(CHtmlListDlg::IDD, CHtmlListDlg::IDH, pParent)
, m_curStyle(_T(""))
, m_nSelRowIdx(0)
, m_pSelITable(NULL)
, m_sSelId(NULL)
, m_ptSelFolderList(NULL)
{
	ptFileList = new CFileInfo;
	

}

CHtmlListDlg::~CHtmlListDlg()
{
}

void CHtmlListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CHtmlListDlg::OnInitDialog()
{
	SetHostFlags(DOCHOSTUIFLAG_FLAT_SCROLLBAR);
	CDHtmlDialog::OnInitDialog();

	CTempBuffer<WCHAR, MAX_PATH> strBuffer(MAX_PATH);
	strBuffer[0] = _T('\0');
	DWORD length = GetCurrentDirectory(MAX_PATH, strBuffer);

	if (length > MAX_PATH)
	{
		// We need to reallocate the string => MAX_PATH wasn't space enough.
		strBuffer.Reallocate(length);
		strBuffer[0] = _T('\0');
		GetCurrentDirectory(length, strBuffer);
	}
	CString spath(strBuffer);
	if (spath.Right(1).Compare(L"\\")!=0)
	{
		spath +=L"\\";
	}

	//AfxMessageBox(CString(spath));
	CString strStyle = spath + _T("liststyle.htm");
	//AfxMessageBox(strStyle);
	m_curStyle = strStyle;
	WIN32_FIND_DATA finddata;
	HANDLE hFind = FindFirstFile(m_curStyle, &finddata);
	if (hFind==INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"未找到文件");
		//LoadFromResource(IDR_HTML_HTMLLISTDLG);
	}
	else
	{
		Navigate( m_curStyle ) ; //加载自定义样式，失败则加载默认样式
	}

	//InitHtml();//加载资源


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BEGIN_MESSAGE_MAP(CHtmlListDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CHtmlListDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
	DHTML_EVENT_ONCLICK(_T("browsestyle"), OnBrowseStyle)
	DHTML_EVENT_ONCLICK(_T("SortUp"), OnSortUp)
	DHTML_EVENT_ONCLICK(_T("SortDown"), OnSortDown)
	DHTML_EVENT_ONCLICK(_T("SortUpCur"), OnSortUpCur)
	DHTML_EVENT_ONCLICK(_T("SortDownCur"), OnSortDownCur)
	DHTML_EVENT_ONCLICK(_T("ListSort"), OnListSort)
	DHTML_EVENT_ONCLICK(_T("unfold"), OnUnfold)
	DHTML_EVENT_ONCLICK(_T("fold"), OnFold)
	//DHTML_EVENT_ELEMENT(DISPID_HTMLELEMENTEVENTS_ONCLICK, _T("sortup"), OnSortUp) // id为elementid的元素事件响应

	//DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONMOUSEOVER, _T("listh"),  OnMouseOverElement)
	//DHTML_EVENT_CLASS(DISPID_HTMLELEMENTEVENTS_ONMOUSEOUT, _T("listh"),  OnMouseOutElement)
END_DHTML_EVENT_MAP()



// CHtmlListDlg 消息处理程序

HRESULT CHtmlListDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CHtmlListDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}
HRESULT CHtmlListDlg::OnBrowseStyle(IHTMLElement* /*phtmlElement*/)
{
	//BROWSEINFO bi;
	//TCHAR szDisplayName[MAX_PATH];
	//memset(&bi, 0x00, sizeof(bi));
	//bi.hwndOwner = m_hWnd;
	//bi.pszDisplayName = szDisplayName;
	//bi.ulFlags = BIF_STATUSTEXT|BIF_NEWDIALOGSTYLE |BIF_RETURNONLYFSDIRS|BIF_EDITBOX;//BIF_RETURNONLYFSDIRS;
	//LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	//if (pidl)
	//{
	//	SHGetPathFromIDList(pidl, szDisplayName);
	//	AfxMessageBox(CString(szDisplayName));
	//	//CString strStyle = strBuffer + _T("liststyle.htm");
	//	Navigate( szDisplayName ) ; 
	//	//RefreshDir();

	//	// free the pidl
	//	CComPtr<IMalloc> spSHMalloc;
	//	if(FAILED(SHGetMalloc(&spSHMalloc)))
	//	{
	//		return S_FALSE;
	//	}
	//	if (spSHMalloc != NULL)
	//		spSHMalloc->Free(pidl);
	//}
	CString filter = _T("所有文件|*.*||");
	CFileDialog mydialog(TRUE,0,0, OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, filter);
	if( mydialog.DoModal() == IDOK )
	{
		m_curStyle = mydialog.GetPathName();
		Navigate( m_curStyle );
		//重新显示数据
		

	}

	return S_OK;
}
void CHtmlListDlg::BackupData()
{
	CFileDialog dlg(0,_T("dat"),0,OFN_OVERWRITEPROMPT);
	if(dlg.DoModal()==IDCANCEL)
	{
		return;
	}
	CFile file;
	file.Open(dlg.GetPathName(), CFile::modeReadWrite|CFile::modeCreate);
	CArchive ar(&file,CArchive::store);
	ptFileList->Serialize(ar);
	ar.Close();
	file.Close();
}
int CHtmlListDlg::LoadFileinfoData()
{
	CFileDialog dlg(TRUE,_T("dat"));// TRUE for FileOpen, FALSE for FileSaveAs
	if(dlg.DoModal()==IDCANCEL)
	{
		return 1;
	}
	CFile file;
	if(!file.Open(dlg.GetPathName(), CFile::modeRead))
	{
		return 1;
	}
	CArchive ar(&file,CArchive::load);
	if (ptFileList != NULL)
	{
		ptFileList->Release();
		ptFileList = NULL;
	}
	ptFileList = new CFileInfo;
	ptFileList->Serialize(ar);
	ar.Close();
	file.Close();
	return 0;
}
int CHtmlListDlg::AddList(CFileInfo* ptfileinfo)
{
	ptFileList->children.push_back( ptfileinfo );
	RefreshDisplay();
	return 0;
}

int CHtmlListDlg::RefreshDisplay(void)
{
	InsertFolderRow();
	return 0;
}

HRESULT  CHtmlListDlg::OnSortUp(IHTMLElement *phtmlElement)
{
	AfxMessageBox(L"sortup");
	if (m_spOldSort !=NULL)
	{
		m_spOldSort->put_outerHTML(objHtml[IDSORTUP].sOuterHtml);
		//m_spCurrElement
		//AfxMessageBox(L"ddd");
	}

	phtmlElement->put_outerHTML(objHtml[IDSORTUPCUR].sOuterHtml);

	m_spOldSort = phtmlElement;
	return S_OK;
}
HRESULT  CHtmlListDlg::OnSortDown(IHTMLElement *phtmlElement)
{
	AfxMessageBox(L"sortdown");
	if (m_spOldSort !=NULL)
	{
		m_spOldSort->put_outerHTML(objHtml[IDSORTUP].sOuterHtml);
		//m_spCurrElement->Release();
		//AfxMessageBox(L"ddd");
	}
	phtmlElement->put_outerHTML(objHtml[IDSORTDOWNCUR].sOuterHtml);
	m_spOldSort = phtmlElement;
	return S_OK;
}
HRESULT  CHtmlListDlg::OnSortUpCur(IHTMLElement *phtmlElement)
{
	if (m_spOldSort !=NULL)
	{
		m_spOldSort->put_outerHTML(objHtml[IDSORTUP].sOuterHtml);
		//m_spCurrElement->Release();
		//AfxMessageBox(L"ddd");
	}
	m_spOldSort = phtmlElement;
	return S_OK;
}
HRESULT  CHtmlListDlg::OnSortDownCur(IHTMLElement *phtmlElement)
{
	if (m_spOldSort !=NULL)
	{
		m_spOldSort->put_outerHTML(objHtml[IDSORTUP].sOuterHtml);
		//m_spCurrElement->Release();
		//AfxMessageBox(L"ddd");
	}
	m_spOldSort = phtmlElement;
	return S_OK;
}

HRESULT  CHtmlListDlg::OnListSort(IHTMLElement *phtmlElement)
{
	AfxMessageBox(L"listsort");
	return S_OK;
}

HRESULT CHtmlListDlg::OnMouseOverElement(IHTMLElement *phtmlElement)
{
	IHTMLStyle *phtmlStyle;
	phtmlElement->get_style(&phtmlStyle);
	if (phtmlStyle)
	{
		VARIANT varColor;
		varColor.vt = VT_I4;
		varColor.lVal = 0xff0000;
		phtmlStyle->put_textDecorationUnderline(VARIANT_TRUE);
		phtmlStyle->put_color(varColor);
		phtmlStyle->put_cursor(L"hand");
		phtmlStyle->Release();
	}
	m_spCurrElement = phtmlElement;
	//long lx,ly;
	//phtmlElement->get_offsetLeft(&lx);
	//phtmlElement->get_offsetTop(&ly);
	CString sdivsort;
	//sdivsort.Format(L"%d,%d",lx,ly);
	//AfxMessageBox(sdivsort);
	sdivsort=_T("<DIV id=sortstyle style=\"align:right;position:absolute;background-color:#0000cc;border:1px solid #cccccc;\">")
		_T("<span id=SortUp class=btn title=升序>△</span>")
		_T("<span id=SortDown class=btn title=降序>▽</span>")
		_T("<span id=SortUpCur class=btn title=当前升序>▲</span>")
		_T("<span id=SortDownCur class=btn title=当前降序>▼</span>")
		_T("</DIV>");
	//AfxMessageBox(sdivsort);
	//sdivsort.Format(sdivsort,lx,ly);left:%d;top:%d;

	CComBSTR bstr;
	phtmlElement->get_innerHTML(&bstr);
	m_curInnerHtml = bstr;
	bstr.Append(sdivsort);
	//AfxMessageBox(bstr);
	phtmlElement->put_innerHTML(bstr);
	//SetCapture():
	//phtmlElement->insertAdjacentHTML()
	SysFreeString(bstr);

	return S_OK;
}

HRESULT CHtmlListDlg::OnMouseOutElement(IHTMLElement *phtmlElement)
{
	IHTMLStyle *phtmlStyle;
	phtmlElement->get_style(&phtmlStyle);
	if (phtmlStyle)
	{
		VARIANT varColor;
		varColor.vt = VT_I4;
		varColor.lVal = 0x000000;
		phtmlStyle->put_textDecorationUnderline(VARIANT_FALSE);
		phtmlStyle->put_color(varColor);
		phtmlStyle->Release();
	}
	phtmlElement->put_innerHTML(m_curInnerHtml);
	m_spCurrElement = NULL;
	return S_OK;
}
HRESULT CHtmlListDlg::OnUnfold(IHTMLElement *phtmlElement)
{
	//AfxMessageBox(L"展开");
	ParseSelectRow(phtmlElement);
	m_ptSelFolderList->bExpend = TRUE;
	InsertFolderRow();
	//m_ptSelFolderList = NULL;
	m_nSelRowIdx = -1;
	m_pSelITable = NULL;
	m_sSelId = NULL;

	//更改必须放在解析之后
	phtmlElement->put_outerHTML(objHtml[IDFOLD].sOuterHtml);
	return S_OK;
}
HRESULT CHtmlListDlg::OnFold(IHTMLElement *phtmlElement)
{
	//AfxMessageBox(L"折叠");
	ParseSelectRow(phtmlElement);
	m_ptSelFolderList->bExpend = FALSE;
	HRESULT hr= m_pSelITable->deleteRow(m_nSelRowIdx+1);
	ASSERT(hr==S_OK);
	//m_ptSelFolderList = NULL;
	m_nSelRowIdx = -1;
	m_pSelITable = NULL;
	m_sSelId = NULL;
	phtmlElement->put_outerHTML(objHtml[IDUNFOLD].sOuterHtml);
	return S_OK;
}
int CHtmlListDlg::InitHtml(void)
{
	CComPtr<IHTMLElement> spfileList;
	GetElementInterface(_T("sortstyle"), IID_IHTMLElement, (void **) &spfileList);

	CComPtr<IHTMLDocument3> sphtmldoc3;
	IDispatch* pIDis=NULL;
	spfileList->get_document(&pIDis);
	if (pIDis != NULL) 
	{
		pIDis->QueryInterface(IID_IHTMLDocument3,(void**)&sphtmldoc3);
		IHTMLElement *phtmlElement = NULL;
		for(int i=0;i<IDMAX;i++)
		{
			if( sphtmldoc3->getElementById(objHtml[i].sId,&phtmlElement) == S_OK )//重复ID只取第一个
			{
				if (phtmlElement!=NULL)
				{
					phtmlElement->get_outerHTML(&(objHtml[i].sOuterHtml));
					//AfxMessageBox(objHtml[i].sOuterHtml);
				}
			}
		}
	}
	// 		int dd = GetElement(objHtml[i].sId,&phtmlElement);//重复ID时出错
	return 0;
}

//解决Navigate异步加载问题，有多次触发，根据url判断
void CHtmlListDlg::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	CDHtmlDialog::OnDocumentComplete(pDisp, szUrl);

	if (m_curStyle.CompareNoCase(szUrl)==0)
	{
		InitHtml();
		RefreshDisplay();
	}
}

CString CHtmlListDlg::GetSelFolderTable(CString sID)
{
	//AfxMessageBox(sID);
	//CString sID;
	//if (m_sSelId!=NULL)
	//{
	//	sID = m_sSelId;
	//	sID += L".";
	//}
	if (!sID.IsEmpty())
	{
		sID += L".";
	}
	//不是数字，返回

	//CString sID=L"100.2.5.6.0";
	
	//m_ptSelFolderList = 
	CFileInfo * ptSelList = ptFileList;
	int nStart = 0;
	int nDotIdx = sID.Find(L".",nStart);
	CString sIdx;
	int nIdx = -1;
	while(nDotIdx != -1)
	{
		sIdx = sID.Mid(nStart,nDotIdx-nStart);
		nIdx = _wtoi(sIdx.GetString());
		if (nIdx>0)
		{
			ptSelList = ptSelList->children[nIdx-1];
		}
		nStart = nDotIdx + 1;
		nDotIdx = sID.Find(L".",nStart);
	}
	m_nSelRowIdx = nIdx;

	if (m_nSelRowIdx == -1)
	{
		m_nSelRowIdx = 0;
	}


	CString strTable;
	strTable = _T("<TABLE id=tab");
	strTable += sID + L"0";//2.2.0
	strTable += _T(" border=1px width=100%>\n");

	strTable += _T("<THEAD");
	if (nIdx=-1)
	{
		strTable += _T(" style=\"display:none\"");
	}
	strTable += _T(">");
	strTable += objHtml[IDTABHEAD].sOuterHtml;
	strTable += _T("</THEAD>");

	CString strtmp;
	CString strTime;
	CString strformat = _T("<TR id=%s%d>")
		_T("<TD width=20>%s%d</TD>")
		_T("<TD>%s</TD>") //2013-08-23 18:22:15
		_T("<TD>%s</TD>") //文件夹、文件
		_T("<TD>%s</TD>") //复制、剪切、删除
		_T("<TD>%s</TD>") //D:\新建文件夹\删
		_T("<TD>%s</TD>") //abc.txt
		_T("<TD>%s</TD>") //F:\文本
		_T("<TD>%s</TD>") //abc（2）.txt
		_T("<TD>")
		_T("<DIV id=Ratebar width=100%% style=\"text-align:left;border:1px solid #B1D632; padding: 0px;\">")
		_T("<div style=\"align:left;position:absolute;background-color:#0099cc;width:%d%%;z-index:-10;\">") //10
		_T("</div>")
		_T("<div width=100%% style=\"text-align:center;\">%d%%</div>") //10
		_T("</DIV>")
		_T("</TD>")
		_T("<TD>%d / %d</TD>")
		_T("<TD>%s</TD>") //成功、失败、取消、等待、暂停、40%
		_T("<TD>%s</TD>") //2013-08-23 18:22:18
		_T("<TD>%d</TD>")
		_T("<TD>%s</TD>") //【开始、暂停】、【取消、恢复】、【撤销、重做】
		_T("</TR>");
	for (int i=0;i<ptSelList->children.size();i++)
	{
			strtmp.Format( strformat
				,sID
				,i+1
				,sID
				,i+1
				,ptSelList->children[i]->ftTimeStart.Format(_T("%Y-%m-%d %H:%M:%S"))
				,ptSelList->children[i]->bFolder ? (objHtml[(ptSelList->children[i]->bExpend ? IDFOLD : IDUNFOLD)].sOuterHtml) : _T("文件")
				,_T("复制")
				,ptSelList->children[i]->szSrcDir
				,ptSelList->children[i]->szSrcName
				,ptSelList->children[i]->szDstDir
				,ptSelList->children[i]->szDstName
				,45
				,45
				,ptSelList->children[i]->nTotalSize - ptSelList->children[i]->nCurSize
				,ptSelList->children[i]->nTotalSize
				,_T("成功")
				,ptSelList->children[i]->ftTimeEnd.Format(_T("%Y-%m-%d %H:%M:%S"))
				,ptSelList->children[i]->dwDuration
				,_T("【暂停】")
				,_T("1.0"));
		
		//AfxMessageBox(strtmp);
		strTable += strtmp;
		if (ptSelList->children[i]->bExpend)
		{
			CString sID2;
			sID2.Format(L"%s%d",sID, i+1);
			//AfxMessageBox(sID2);
			strtmp.Format(L"<TR><TD id=%s.0 colSpan=50>%s</TD></TR>", sID2, GetSelFolderTable(sID2));
			//AfxMessageBox(strtmp);
			strTable += strtmp;
		}
	}
	strTable += _T("</TABLE>");
	//AfxMessageBox(strTable);
	//CComBSTR bstrTable = strTable;
	m_ptSelFolderList = ptSelList;
	return strTable;
}

int CHtmlListDlg::InsertFolderRow(void)
{
	if (m_pSelITable == NULL)
	{
		CComPtr<IHTMLElement> spfileList;
		GetElementInterface(_T("fileList"), IID_IHTMLElement, (void **) &spfileList);
		CComBSTR bsStr = GetSelFolderTable(CString(m_sSelId));
		spfileList->put_innerHTML(bsStr);
		return 0;
	}
	IDispatch *disrow;
	HRESULT hr= m_pSelITable->insertRow(m_nSelRowIdx+1,&disrow);
	if(hr!=S_OK) return 1;
	IHTMLTableRow *pIRow;
	hr=disrow->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
	ASSERT(hr==S_OK);
	IDispatch *discell;
	IHTMLElement *cell;

	CComBSTR bsStr = GetSelFolderTable(CString(m_sSelId));

	pIRow->insertCell(0,&discell);
	discell->QueryInterface(IID_IHTMLElement,(void **)&cell);
	cell->put_innerHTML(bsStr);
	CString sTDid = m_sSelId;
	sTDid += L".0";
	CComBSTR bsId = sTDid;
	cell->put_id( bsId );
	IHTMLTableCell *picell;
	hr=cell->QueryInterface(IID_IHTMLTableCell,(void **)&picell);
	picell->put_colSpan(50);
	picell->Release();
	cell->Release();
	discell->Release();
	SysFreeString(bsId);
	//SysFreeString(bsStr);
	pIRow->Release();
	disrow->Release();
	m_pSelITable->Release();
	return 0;
}

HRESULT CHtmlListDlg::ParseSelectRow(IHTMLElement *phtmlElement)
{
	IHTMLElement *pTR;
	HRESULT hr = phtmlElement->get_parentElement(&pTR);
	BSTR stag;
	BSTR ss;
	while (hr==S_OK && pTR!=NULL)
	{
		pTR->get_outerHTML(&ss);
		//AfxMessageBox(ss);
		pTR->get_tagName(&stag);
		if (CString(stag).CompareNoCase(L"TR")==0)
		{
			break;
		}
		hr = pTR->get_parentElement(&pTR);
	}
	if (pTR != NULL)
	{
		hr = pTR->get_id(&m_sSelId);
		if (hr==S_OK)
		{
			//AfxMessageBox(m_sSelId);
		}
		IHTMLTableRow *pIRow;
		hr=pTR->QueryInterface(IID_IHTMLTableRow, (void**)&pIRow);
		ASSERT(hr==S_OK);
// 		hr=pIRow->get_rowIndex(&m_nSelRowIdx);
// 		ASSERT(hr==S_OK);
		//TD->TR-> *TBODY*-> TABLE
		while (hr==S_OK && pTR!=NULL)
		{
			pTR->get_outerHTML(&ss);
			//AfxMessageBox(ss);
			pTR->get_tagName(&stag);
			if (CString(stag).CompareNoCase(L"table")==0)
			{
				break;
			}
			hr = pTR->get_parentElement(&pTR);
		}
		ASSERT(hr==S_OK);
		pTR->get_outerHTML(&ss);
		//AfxMessageBox(ss);
		hr=pTR->QueryInterface(IID_IHTMLTable, (void**)&m_pSelITable);
		ASSERT(hr==S_OK);

		CString sID(m_sSelId);
		if (!sID.IsEmpty())
		{
			sID += L".";
		}
		CFileInfo * ptSelList = ptFileList;
		int nStart = 0;
		int nDotIdx = sID.Find(L".",nStart);
		CString sIdx;
		int nIdx;
		while(nDotIdx != -1)
		{
			sIdx = sID.Mid(nStart,nDotIdx-nStart);
			nIdx = _wtoi(sIdx.GetString());
			if (nIdx>0)
			{
				ptSelList = ptSelList->children[nIdx-1];
			}
			nStart = nDotIdx + 1;
			nDotIdx = sID.Find(L".",nStart);
		}
		m_nSelRowIdx = nIdx;
		m_ptSelFolderList = ptSelList;
	}
	return S_OK;
}
