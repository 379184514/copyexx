
// copyExxDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "HtmlListDlg.h"


// CcopyExxDlg �Ի���
class CcopyExxDlg : public CDialog
{
// ����
public:
	CcopyExxDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_COPYEXX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckLog();
	afx_msg void OnBnClickedCheckHook();
	afx_msg void OnBnClickedCheckList();
	int CheckChange(void);
	BOOL bCkLog;
	BOOL bCkHook;
	BOOL bCkList;
	afx_msg LRESULT OnMyFileOp(WPARAM, LPARAM); //hook������Ϣ
	HMODULE m_hdll;
	int HookExplorer(void);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	CHtmlListDlg m_htmllistdlg;
	ULONGLONG SearchFolder(CString sPath,CFileInfo* pfolder);
};
