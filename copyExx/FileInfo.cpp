#include "StdAfx.h"
#include "FileInfo.h"

CFileInfo::CFileInfo(void)
{
	pParent = NULL;
	ftTimeStart = 0; // ��ʼʱ��
	ftTimeEnd = 0; // ����ʱ��
	nTotalSize = 0; // �ļ����ֽ���
	nCurSize = 0;//��ǰ�ֽ���
	szSrcDir = _T("");//Դ·��
	szSrcName = _T("");//Դ�ļ���
	szDstDir = _T("");//Ŀ��·��
	szDstName = _T("");//Ŀ���ļ���
	dwDuration = 0; // �ܹ���ʱ����������ͣ��ʱ��
	nAction = 0;//�������ͣ����ơ����С�ɾ�������������½�
	nResult = 0;//������ɹ���ʧ�ܡ�ȡ�����ȴ�����ͣ�����ǡ����������ϲ�������������
	bFolder = FALSE;//�Ƿ����ļ���
	bExpend = FALSE;//�ļ����Ƿ��۵�
}

CFileInfo::~CFileInfo(void)
{
	Release();
}

int CFileInfo::Release(void)
{
	for(int i=0;i<children.size();i++)
	{
		delete children[i];
	}
	children.clear();
	return 0;
}

int CFileInfo::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << ftTimeStart;
		ar << ftTimeEnd;
		ar << nTotalSize;
		ar << nCurSize;
		ar << szSrcDir;
		ar << szSrcName;
		ar << szDstDir;
		ar << szDstName;
		ar << dwDuration;
		ar << nAction;
		ar << nResult;
		ar << bFolder;
		ar << bExpend;
		ar << (int)children.size();

		for(int i=0;i<children.size();i++)
		{
			children[i]->Serialize(ar);
		}
	}
	else
	{
		Release();
		ar >> ftTimeStart;
		ar >> ftTimeEnd;
		ar >> nTotalSize;
		ar >> nCurSize;
		ar >> szSrcDir;
		ar >> szSrcName;
		ar >> szDstDir;
		ar >> szDstName;
		ar >> dwDuration;
		ar >> nAction;
		ar >> nResult;
		ar >> bFolder;
		ar >> bExpend;
		int siz;
		ar >> siz;
		children.resize(siz);

		for(int i=0;i<siz;i++)
		{
			children[i] = new CFileInfo;
			children[i]->pParent = this;
			children[i]->Serialize(ar);
		}
	}
	return 0;
}
