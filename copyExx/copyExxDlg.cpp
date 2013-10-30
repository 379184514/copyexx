
// copyExxDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "copyExx.h"
#include "copyExxDlg.h"
#include "..\fotracker\ifileophook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define WM_MYFILEOP (WM_USER+379)
//#define FILE_OP	379
//#define DLL_HOOKED 184
//#define DLL_UNHOOK 514


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CcopyExxDlg �Ի���




CcopyExxDlg::CcopyExxDlg(CWnd* pParent /*=NULL*/)
: CDialog(CcopyExxDlg::IDD, pParent)
, bCkLog(TRUE)
, bCkHook(TRUE)
, bCkList(TRUE)
, m_hdll(NULL)
, m_htmllistdlg(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcopyExxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_LOG, bCkLog);
	DDX_Check(pDX, IDC_CHECK_HOOK, bCkHook);
	DDX_Check(pDX, IDC_CHECK_LIST, bCkList);
}
//UINT uMsgMyFileOp = ::RegisterWindowMessage(L"uMsgMyFileOp");
BEGIN_MESSAGE_MAP(CcopyExxDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_LOG, &CcopyExxDlg::OnBnClickedCheckLog)
	ON_BN_CLICKED(IDC_CHECK2, &CcopyExxDlg::OnBnClickedCheckHook)
	ON_BN_CLICKED(IDC_CHECK_LIST, &CcopyExxDlg::OnBnClickedCheckList)
	//ON_MESSAGE(WM_MYFILEOP, OnMyFileOp)
	ON_REGISTERED_MESSAGE(uMsgMyFileOp, OnMyFileOp)
	ON_BN_CLICKED(IDOK, &CcopyExxDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CcopyExxDlg::OnBnClickedCancel)
	ON_WM_COPYDATA()
END_MESSAGE_MAP()


// CcopyExxDlg ��Ϣ�������

BOOL CcopyExxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	////--���ñ�ǩ��ʾ�ؼ� S----//
	//EnableToolTips(TRUE); 
	//m_tooltip_filelist.Create(this); 
	//m_tooltip_filelist.Activate(TRUE); 
	//CWnd* pW=GetDlgItem(IDC_BTN_DEL1);//�õ�����ָ�� 
	//m_tooltip_filelist.AddTool(pW,_T("ɾ������ѡ����")); //��ӹ̶��ı�
	////--���ñ�ǩ��ʾ�ؼ� E----//

	HookExplorer();

	CString strSrc = _T("c:\\CopyHook.dll");
	CFileStatus status;
//	CFile::GetStatus(strSrc, status);

	m_htmllistdlg.Create(IDD_HTMLLIST_DIALOG,GetDesktopWindow()); 
	//m_htmllistdlg.ShowWindow(SW_SHOW);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CcopyExxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CcopyExxDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CcopyExxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CcopyExxDlg::OnBnClickedCheckLog()
{
	CheckChange();
}

void CcopyExxDlg::OnBnClickedCheckHook()
{
	CheckChange();
}

void CcopyExxDlg::OnBnClickedCheckList()
{
	CheckChange();
}

int CcopyExxDlg::CheckChange(void)
{
	UpdateData(TRUE);
	if(bCkLog || bCkHook)
	{
		//hook
		if (!m_hdll)
		{
			HookExplorer();
		}

	}
	else
	{
		//ж��dll,���hook
		if (m_hdll)
		{
			//SetDlgItemText(IDC_LOADDLL,_T("���hook��") );
			void (WINAPI*UninstallGlobalHook)();
			(FARPROC&)UninstallGlobalHook = GetProcAddress(m_hdll,"UninstallGlobalHook");
			UninstallGlobalHook();

			if ( FreeLibrary(m_hdll) )
			{
				m_hdll = NULL;
				//SetDlgItemText(IDC_LOADDLL,_T("���hook��") );
			}
			else
			{
				//SetDlgItemText(IDC_LOADDLL,_T("���hookʧ�ܣ�") );
			}
		}
	}
	if(bCkList)
	{
		//��ʾ�б�
		m_htmllistdlg.ShowWindow(SW_SHOW);
//#ifndef DEBUG
		//m_htmllistdlg.RefreshDisplay();
//#endif
		
	}
	else
	{
		//�����б�
		m_htmllistdlg.ShowWindow(SW_HIDE);
	}
	return 0;
}

//	::SendMessage(GetParent()->m_hWnd,WM_VSCROLL,MAKELONG(SB_THUMBPOSITION,nPos),(LPARAM)m_hWnd);
LRESULT CcopyExxDlg::OnMyFileOp(WPARAM wParam, LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(wParam);
	//UNREFERENCED_PARAMETER(lParam);

	UINT nType = (UINT)wParam;
	//AfxMessageBox(_T("opmsg"));
 	CString ss;
// 	ss.Format(L"ntype:%d",nType);
// 	AfxMessageBox(ss);
	switch(nType)
	{
	case DLL_HOOKED:
		SetDlgItemText(IDC_LOADDLL, _T("explorer hooked��"));
		break;
	case DLL_UNHOOK:
		SetDlgItemText(IDC_LOADDLL, _T("explorer unhook��"));
		break;
	case FILE_OP:
		{
// 			AfxMessageBox(L"FILE_OP");
// 			ss.Format(L"lParam:%d",lParam);
// 			AfxMessageBox(ss);
// 			PFileOPItem pfoi = (PFileOPItem)lParam;
// 			ss.Format(L"count:%d",pfoi->srcCount);
			// 			AfxMessageBox(ss);
			// 			for (UINT i=0;i<(*pfoi)->srcCount;i++)
			// 			{
			// 				AfxMessageBox(L"exx");
			// 				AfxMessageBox((*pfoi)->srcList[i]);
			// 			}
			// 			AfxMessageBox(L"00��");
			// 			AfxMessageBox((*pfoi)->dstFolder);

		}
		break;
	default:
		break;
	}
	return 0;
}

int CcopyExxDlg::HookExplorer()
{
	//if (!m_hdll)
	{
		CString dllname; 
		CString sysname;
		SYSTEM_INFO info;                //��SYSTEM_INFO�ṹ�ж�64λAMD������    
		GetSystemInfo(&info);            //����GetSystemInfo�������ṹ    
		OSVERSIONINFOEX os;   
		os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);    
		if(GetVersionEx((OSVERSIONINFO *)&os))                     
		{   
			switch(os.dwMajorVersion)
			{                        //�ж����汾��    //switch(os.dwMinorVersion){                //�жϴΰ汾��  
			case 5:								//2000,xp,2003
				dllname = _T("xpfotracker.dll");
				sysname = _T("windows 2000/xp/2003");
				break;
			case 6:								//vista,win7,win8
				if (info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					//if(IsWow64())//32��exe��x64����wow64ģ��32
				{
					dllname = _T("fotracker64.dll");
					sysname = _T("vista,win7,win8  x64");
				}
				else
				{
					dllname = _T("fotracker.dll");
					sysname = _T("vista,win7,win8  x86");
				}
				break;

			default:
				dllname = _T("");
				sysname = _T("��֧��");
				break;
			}
		}

		SetDlgItemText(IDC_SYSCPU,sysname);
		m_hdll=LoadLibrary(dllname);
		if(!m_hdll)
		{
			SetDlgItemText(IDC_LOADDLL,CString(_T("����") + dllname +_T("ʧ�ܣ�")) );
			// 			GetDlgItem(IDC_CHECK_LOG)->EnableWindow(FALSE);
			// 			GetDlgItem(IDC_CHECK_HOOK)->EnableWindow(FALSE);
			// 			GetDlgItem(IDC_CHECK_LIST)->EnableWindow(FALSE);
			return 1;
		}
		SetDlgItemText(IDC_LOADDLL,CString(_T("����") + dllname +_T("�ɹ���")) );
	}

	void  (WINAPI*EnterInjectMode)();
	(FARPROC&)EnterInjectMode=GetProcAddress(m_hdll,"EnterInjectMode");
	EnterInjectMode();
	return 0;
}

void CcopyExxDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CcopyExxDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

BOOL CcopyExxDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	int nDataSize = pCopyDataStruct->cbData;   

	if(nDataSize > 0)   
	{   
		PFileOPItem pfoi = new FileOPItem();
		memset(pfoi->dstFolder, 0, MAX_PATH * sizeof(WCHAR));
		pfoi->srcCount = 0;
		pfoi->srcList = NULL;
		pfoi->opType = UNDEF;
		memcpy(pfoi, pCopyDataStruct->lpData, sizeof(*pfoi));   
		//::MessageBox(m_hWnd, pfoi->dstFolder, L"123", MB_OK);
		pfoi->srcList = (WPATH*)malloc(pCopyDataStruct->cbData - sizeof(*pfoi));
		memcpy(pfoi->srcList, (BYTE*)pCopyDataStruct->lpData + sizeof(*pfoi), pCopyDataStruct->cbData - sizeof(*pfoi));
		CTime stTime(CTime::GetCurrentTime());
		//stTime.GetCurrentTime();
		for (UINT i=0;i<pfoi->srcCount;i++)
		{
			//::MessageBoxW(NULL,pfoi->srcList[i],L"123",MB_OK);
			CString strSrc = pfoi->srcList[i];
			//CFile::GetStatus()
			WIN32_FIND_DATA fdata;
			HANDLE hfind = ::FindFirstFile( strSrc, &fdata );
			if(hfind != INVALID_HANDLE_VALUE)
			{	
				CFileInfo* pfile = new CFileInfo;
				if( fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{
					pfile->bFolder = TRUE;
					pfile->nTotalSize = SearchFolder(strSrc,pfile);
				}
				else
				{
					pfile->bFolder = FALSE;
					pfile->nTotalSize = fdata.nFileSizeHigh;
					pfile->nTotalSize <<= 32;
					pfile->nTotalSize |= fdata.nFileSizeLow;
				}
				int nsplit = strSrc.ReverseFind(_T('\\'));
				pfile->pParent = NULL;
				pfile->dwDuration = 0;
				pfile->ftTimeEnd = stTime;
				pfile->ftTimeStart = stTime;
				pfile->nAction = pfoi->opType;
				pfile->nCurSize = 0;
				pfile->nResult = 0;
				pfile->szDstDir = pfoi->dstFolder;
				pfile->szDstName = strSrc.Mid(nsplit+1);
				pfile->szSrcDir = strSrc.Left(nsplit+1);
				pfile->szSrcName = strSrc.Mid(nsplit+1);
				m_htmllistdlg.AddList(pfile);

				FindClose(hfind);
			}
		}
		free(pfoi->srcList);
		delete [] pfoi;   
	} 


	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

ULONGLONG CcopyExxDlg::SearchFolder(CString sPath,CFileInfo* pfolder)
{
	ULONGLONG nFolderSize = 0;
	CTime stTime(CTime::GetCurrentTime());
	int nLastDot = -1;
	if( sPath.Right(1) != _T("\\") )
	{
		sPath += _T("\\");
	}
	WIN32_FIND_DATA fdata;
	HANDLE hfind = ::FindFirstFile( sPath + _T("*"), &fdata );
	if(hfind != INVALID_HANDLE_VALUE)
	{	
		do
		{
			CFileInfo* pfile = new CFileInfo;
			CString strSrc = sPath + fdata.cFileName;
			if( fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
			{
				if(_tccmp(fdata.cFileName,_T(".")) ==0 || _tccmp(fdata.cFileName,_T("..")) == 0)
				{
					pfile->Release();
					continue;
				}
				pfile->bFolder = TRUE;
				pfile->nTotalSize = SearchFolder(strSrc,pfile);
			}
			else
			{
				pfile->bFolder = FALSE;
				pfile->nTotalSize = fdata.nFileSizeHigh;
				pfile->nTotalSize <<= 32;
				pfile->nTotalSize |= fdata.nFileSizeLow;
			}
			pfile->bExpend = FALSE;
			pfile->pParent = pfolder;
			pfile->dwDuration = 0;
			pfile->ftTimeEnd = stTime;
			pfile->ftTimeStart = stTime;
			pfile->nAction = pfolder->nAction;
			pfile->nCurSize = 0;
			pfile->nResult = 0;
			pfile->szDstDir = pfolder->szDstDir + pfolder->szDstName;
			pfile->szDstName = fdata.cFileName;
			pfile->szSrcDir = sPath;
			pfile->szSrcName = fdata.cFileName;
			pfolder->children.push_back( pfile );
			nFolderSize += pfile->nTotalSize;
		}while(FindNextFile(hfind,&fdata));

		FindClose(hfind);
	}
	return ULONGLONG(nFolderSize);
}
