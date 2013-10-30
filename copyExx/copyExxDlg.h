
// copyExxDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "HtmlListDlg.h"


// CcopyExxDlg 对话框
class CcopyExxDlg : public CDialog
{
// 构造
public:
	CcopyExxDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_COPYEXX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	afx_msg LRESULT OnMyFileOp(WPARAM, LPARAM); //hook到的消息
	HMODULE m_hdll;
	int HookExplorer(void);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	CHtmlListDlg m_htmllistdlg;
	ULONGLONG SearchFolder(CString sPath,CFileInfo* pfolder);
};
