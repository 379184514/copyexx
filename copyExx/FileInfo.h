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

	CTime ftTimeStart; // ��ʼʱ��
	CTime ftTimeEnd; // ����ʱ��
	ULONGLONG nTotalSize; // �ļ����ֽ���
	ULONGLONG nCurSize;//��ǰ�ֽ���
	CString szSrcDir;//Դ·��
	CString szSrcName;//Դ�ļ���
	CString szDstDir;//Ŀ��·��
	CString szDstName;//Ŀ���ļ���
	DWORD dwDuration; // �ܹ���ʱ����������ͣ��ʱ��
	int nAction;//�������ͣ����ơ����С�ɾ�������������½���
	int nResult;//���,�ɹ���ʧ�ܡ�ȡ�����ȴ�����ͣ�����ǡ����������ϲ�������
	BOOL bFolder;//�Ƿ����ļ���
	BOOL bExpend;//�ļ����Ƿ�չ��
};
