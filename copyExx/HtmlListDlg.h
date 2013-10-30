#pragma once
#include "FileInfo.h"

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 

enum ID_HTML
{
	IDSORTUP = 0,	//升序 △
	IDSORTDOWN,	//降序 ▽
	IDSORTUPCUR,	//当前升序 ▲
	IDSORTDOWNCUR,// 当前降序 ▼
	IDUNFOLD,		//展开 [+]
	IDFOLD,		//折叠>[-]
	IDTABHEAD, //THEAD 列表标题
	IDFILE,//文件
	IDFOLDER,//文件夹
	IDMAX
};

typedef struct ObjHtml
{
	ID_HTML nId;//序号
	BSTR  sId;//ID名称
	CComBSTR sOuterHtml;//
}ObjHtml,*pObjHtml;

// CHtmlListDlg 对话框

class CHtmlListDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CHtmlListDlg)

public:
	CHtmlListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHtmlListDlg();
// 重写
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	HRESULT OnBrowseStyle(IHTMLElement *phtmlElement);
	HRESULT OnSortUp(IHTMLElement *phtmlElement);//
	HRESULT OnSortDown(IHTMLElement *phtmlElement);
	HRESULT OnSortUpCur(IHTMLElement *phtmlElement);
	HRESULT OnSortDownCur(IHTMLElement *phtmlElement);
	HRESULT OnListSort(IHTMLElement *phtmlElement);//只有一个按钮，2种状态切换
	
	HRESULT OnMouseOverElement(IHTMLElement *phtmlElement);
	HRESULT OnMouseOutElement(IHTMLElement *phtmlElement);
	HRESULT OnUnfold(IHTMLElement *phtmlElement);
	HRESULT OnFold(IHTMLElement *phtmlElement);

// 对话框数据
	enum { IDD = IDD_HTMLLIST_DIALOG, IDH = IDR_HTML_HTMLLISTDLG };
	CFileInfo* ptFileList;
	CComPtr<IHTMLElement> m_spCurrElement;
	CComPtr<IHTMLElement> m_spOldSort;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	void BackupData();
	int LoadFileinfoData();
	int AddList(CFileInfo* ptfileinfo);
	int RefreshDisplay(void);
	CComBSTR m_curInnerHtml;
	int InitHtml(void);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	CString m_curStyle;
	BSTR m_sSelId;
	long m_nSelRowIdx;
	IHTMLTable *m_pSelITable;
	CString GetSelFolderTable(CString sID);
	int InsertFolderRow(void);
	HRESULT ParseSelectRow(IHTMLElement *phtmlElement);
	CFileInfo* m_ptSelFolderList;
};
