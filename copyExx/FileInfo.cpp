#include "StdAfx.h"
#include "FileInfo.h"

CFileInfo::CFileInfo(void)
{
	pParent = NULL;
	ftTimeStart = 0; // 开始时间
	ftTimeEnd = 0; // 结束时间
	nTotalSize = 0; // 文件总字节数
	nCurSize = 0;//当前字节数
	szSrcDir = _T("");//源路径
	szSrcName = _T("");//源文件名
	szDstDir = _T("");//目标路径
	szDstName = _T("");//目标文件名
	dwDuration = 0; // 总共用时，不包括暂停的时间
	nAction = 0;//操作类型：复制、剪切、删除、重命名、新建
	nResult = 0;//结果：成功、失败、取消、等待、暂停、覆盖、重命名、合并、跳过、撤销
	bFolder = FALSE;//是否是文件夹
	bExpend = FALSE;//文件夹是否折叠
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
