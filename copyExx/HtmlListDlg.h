#pragma once
#include "FileInfo.h"

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CDHtmlDialog��"
#endif 

enum ID_HTML
{
	IDSORTUP = 0,	//���� ��
	IDSORTDOWN,	//���� ��
	IDSORTUPCUR,	//��ǰ���� ��
	IDSORTDOWNCUR,// ��ǰ���� ��
	IDUNFOLD,		//չ�� [+]
	IDFOLD,		//�۵�>[-]
	IDTABHEAD, //THEAD �б����
	IDFILE,//�ļ�
	IDFOLDER,//�ļ���
	IDMAX
};

typedef struct ObjHtml
{
	ID_HTML nId;//���
	BSTR  sId;//ID����
	CComBSTR sOuterHtml;//
}ObjHtml,*pObjHtml;

// CHtmlListDlg �Ի���

class CHtmlListDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CHtmlListDlg)

public:
	CHtmlListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHtmlListDlg();
// ��д
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	HRESULT OnBrowseStyle(IHTMLElement *phtmlElement);
	HRESULT OnSortUp(IHTMLElement *phtmlElement);//
	HRESULT OnSortDown(IHTMLElement *phtmlElement);
	HRESULT OnSortUpCur(IHTMLElement *phtmlElement);
	HRESULT OnSortDownCur(IHTMLElement *phtmlElement);
	HRESULT OnListSort(IHTMLElement *phtmlElement);//ֻ��һ����ť��2��״̬�л�
	
	HRESULT OnMouseOverElement(IHTMLElement *phtmlElement);
	HRESULT OnMouseOutElement(IHTMLElement *phtmlElement);
	HRESULT OnUnfold(IHTMLElement *phtmlElement);
	HRESULT OnFold(IHTMLElement *phtmlElement);

// �Ի�������
	enum { IDD = IDD_HTMLLIST_DIALOG, IDH = IDR_HTML_HTMLLISTDLG };
	CFileInfo* ptFileList;
	CComPtr<IHTMLElement> m_spCurrElement;
	CComPtr<IHTMLElement> m_spOldSort;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
