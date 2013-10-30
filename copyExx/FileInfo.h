#pragma once
#include <vector>

class CFileInfo
{
public:
	CFileInfo(void);
	~CFileInfo(void);
	CFileInfo* pParent;
	std::vector<CFileInfo*> children;
	int Release(void);
	int Serialize(CArchive& ar);

	CTime ftTimeStart; // 开始时间
	CTime ftTimeEnd; // 结束时间
	ULONGLONG nTotalSize; // 文件总字节数
	ULONGLONG nCurSize;//当前字节数
	CString szSrcDir;//源路径
	CString szSrcName;//源文件名
	CString szDstDir;//目标路径
	CString szDstName;//目标文件名
	DWORD dwDuration; // 总共用时，不包括暂停的时间
	int nAction;//操作类型（复制、剪切、删除、重命名、新建）
	int nResult;//结果,成功、失败、取消、等待、暂停、覆盖、重命名、合并、跳过
	BOOL bFolder;//是否是文件夹
	BOOL bExpend;//文件夹是否展开
};
