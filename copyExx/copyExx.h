
// copyExx.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CcopyExxApp:
// �йش����ʵ�֣������ copyExx.cpp
//

class CcopyExxApp : public CWinApp
{
public:
	CcopyExxApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CcopyExxApp theApp;