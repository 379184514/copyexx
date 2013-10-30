

/*****************************************************************************\
* @file   : IFileOPHook.cpp                                                 *
* @author : 吴永                                                            *
* @date   : 2011.10.27                                                      *
\*****************************************************************************/
#include <shlwapi.h>
#include <shobjidl.h>
#include <stdio.h>
#include "IFileOPHook.h"

#pragma comment(lib, "shlwapi.lib")
#pragma warning(disable:4996)

//UINT uMsgMyFileOp = ::RegisterWindowMessage(L"uMsgMyFileOp");

static const IID CLSID_FileOperation = {0x3ad05575, 0x8857, 0x4850, {0x92, 0x77, 0x11, 0xb8, 0x5b, 0xdb, 0x8e, 0x09}};
static const IID IID_IFileOperation = {0x947aab5f, 0x0a5c, 0x4c13, {0xb4, 0xd6, 0x4b, 0xf7, 0x83, 0x6f, 0xc9, 0xf8}};

struct IFileOperation;



//#define OutputDebugStringA(ss) MessageBoxA(NULL,ss,"Hooked - ANSI",MB_OK);
//#define OutputDebugStringW(ss) MessageBoxW(NULL,ss,L"Hooked - Unicode",MB_OK);



#define QueryInterface_Index 0
#define AddRef_Index                        (QueryInterface_Index + 1)
#define Release_Index                       (AddRef_Index + 1)
#define Advice_Index                        (Release_Index + 1)
#define Unadvise_Index                      (Advice_Index + 1)
#define SetOperationFlags_Index             (Unadvise_Index + 1)
#define SetProgressMessage_Index            (SetOperationFlags_Index + 1)
#define SetProgressDialog_Index             (SetProgressMessage_Index + 1)
#define SetProperties_Index                 (SetProgressDialog_Index + 1)
#define SetOwnerWindow_Index                (SetProperties_Index + 1)
#define ApplyPropertiesToItem_Index         (SetOwnerWindow_Index + 1)
#define ApplyPropertiesToItems_Index        (ApplyPropertiesToItem_Index + 1)
#define RenameItem_Index                    (ApplyPropertiesToItems_Index + 1)
#define RenameItems_Index                   (RenameItem_Index + 1)
#define MoveItem_Index                      (RenameItems_Index + 1)
#define MoveItems_Index                     (MoveItem_Index + 1)
#define CopyItem_Index                      (MoveItems_Index + 1)
#define CopyItems_Index                     (CopyItem_Index + 1)
#define DeleteItem_Index                    (CopyItems_Index + 1)
#define DeleteItems_Index                   (DeleteItem_Index + 1)
#define NewItem_Index                       (DeleteItems_Index + 1)
#define PerformOperations_Index             (NewItem_Index + 1)
#define GetAnyOperationAborted_Index        (PerformOperations_Index + 1)

typedef HRESULT (__stdcall* PCopyItems)(IFileOperation*, IUnknown*, IShellItem*);
typedef HRESULT (__stdcall* PMoveItems)(IFileOperation*, IUnknown*, IShellItem*);
typedef HRESULT (__stdcall* PPerformOperations)(IFileOperation*);

static PCopyItems CopyItems_old = NULL;
static PMoveItems MoveItems_old = NULL;
static PPerformOperations PerformOperations_old = NULL;
static BOOL gs_bHooked = FALSE;
static DWORD gs_opSlot = TLS_OUT_OF_INDEXES;

static size_t WINAPI HookVtbl(void* pObject, size_t classIdx, size_t methodIdx, size_t newMethod)
{
	OutputDebugStringA("1");
	size_t** vtbl = (size_t**)pObject;
	DWORD oldProtect = 0;
	size_t oldMethod = vtbl[classIdx][methodIdx];
	VirtualProtect(vtbl[classIdx] + sizeof(size_t*) * methodIdx, sizeof(size_t*), PAGE_READWRITE, &oldProtect);
	vtbl[classIdx][methodIdx] = newMethod;
	VirtualProtect(vtbl[classIdx] + sizeof(size_t*) * methodIdx, sizeof(size_t*), oldProtect, &oldProtect);
	return oldMethod;
}

static PFileOPItem GetOPItem()
{
	OutputDebugStringA("2");
	if(!TlsGetValue(gs_opSlot))
	{
		PFileOPItem pfoi = new FileOPItem();
		memset(pfoi->dstFolder, 0, MAX_PATH * sizeof(WCHAR));
		pfoi->srcCount = 0;
		pfoi->srcList = NULL;
		pfoi->opType = UNDEF;
		TlsSetValue(gs_opSlot, pfoi);
	}

	return (PFileOPItem)TlsGetValue(gs_opSlot);
}

static OPTYPE GetOPType()
{
	OutputDebugStringA("3");
	return GetOPItem()->opType;
}

static void SetOPType(OPTYPE type)
{
	OutputDebugStringA("4");
	GetOPItem()->opType = type;
}

// 获取一次文件操作中所有文件名
static UINT GetFilesFromDataObject(IUnknown *iUnknown, WPATH **ppPath)
{
	OutputDebugStringA("5");
	UINT uFileCount = 0;
	IDataObject *iDataObject = NULL;
	HRESULT hr = iUnknown->QueryInterface(IID_IDataObject, (void **)&iDataObject);
	OutputDebugStringA("50");
	do
	{
		OutputDebugStringA("51");
		if(!SUCCEEDED(hr))
		{
			break;
		}
		FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stg = { TYMED_HGLOBAL };
		OutputDebugStringA("52");
		if(!SUCCEEDED(iDataObject->GetData(&fmt, &stg)))
		{
			break;
		}
		OutputDebugStringA("53");
		HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
		if(hDrop == NULL)
		{
			break;
		}
		OutputDebugStringA("54");
		uFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		if(uFileCount <= 0)
		{
			break;
		}
		OutputDebugStringA("55");

		*ppPath = new WPATH[uFileCount];
		if(*ppPath != NULL)
		{
			for(UINT uIndex = 0; uIndex < uFileCount; uIndex++)
			{
				DragQueryFile(hDrop, uIndex, (*ppPath)[uIndex], MAX_PATH);
				OutputDebugStringA("从：");
				OutputDebugStringW((*ppPath)[uIndex]);
			}
		}
		else
		{
			uFileCount = 0;
		}

		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
	} while (FALSE);

	return uFileCount;
}

// 释放本线程的文件操作分配的内存
static void FreeOPItem()
{
	OutputDebugStringA("6");
	PFileOPItem pfoi = GetOPItem();
	if(pfoi->srcList)
	{
		delete[] pfoi->srcList;
		pfoi->srcList = NULL;
	}
	pfoi->opType = UNDEF;
	pfoi->srcCount = 0;
	memset(pfoi->dstFolder, 0, MAX_PATH * sizeof(WCHAR));
}

static HRESULT __stdcall CopyItems_new(IFileOperation* pThis,
									   IUnknown* punkItems,
									   IShellItem* psiDestinationFolder)
{
	OutputDebugStringA("7");
	LPWSTR lpDst = NULL;
	psiDestinationFolder->GetDisplayName(SIGDN_FILESYSPATH, &lpDst);
	HRESULT hr = S_FALSE;
	PFileOPItem pfoi = GetOPItem();

	do
	{
		OutputDebugStringA("COPY文件，到：");
		OutputDebugStringW(lpDst);
		pfoi->srcCount = GetFilesFromDataObject(punkItems, &(pfoi->srcList));

		SetOPType(COPY_FILE);
		wcscpy(pfoi->dstFolder, lpDst);
		//::SendMessage(HWND_BROADCAST, uMsgMyFileOp, (WPARAM)FILE_OP,(LPARAM)GetOPItem());
		//::PostMessage(HWND_BROADCAST, uMsgMyFileOp, (WPARAM)FILE_OP,(LPARAM)&pfoi);
		//for (UINT i=0;i<(pfoi)->srcCount;i++)
		//{
		//	//AfxMessageBox((pfoi)->srcList[i]);
		//	 MessageBoxW(NULL,pfoi->srcList[i],L"Hooked - Unicode",MB_OK);
		//}

		//将copy数据合并到连续内存，方便传递
		COPYDATASTRUCT   CDS;
		void* pfoi2 =  malloc( sizeof(FileOPItem) + sizeof(WPATH) * (pfoi->srcCount) );
		//复制pfoi结构体到连续内存前部分
		memcpy(pfoi2, pfoi, sizeof(*pfoi)); 
		//复制pfoi->srcList到连续内存后半部分
		memcpy((BYTE*)pfoi2 + sizeof(FileOPItem), pfoi->srcList, sizeof(WPATH) * (pfoi->srcCount));   
		CDS.dwData = 0;   
		CDS.cbData = sizeof(FileOPItem) + sizeof(WPATH) * (pfoi->srcCount) ; //连续内存总长度
		CDS.lpData = pfoi2;//连续内存起始地址
		::SendMessage(HWND_BROADCAST, WM_COPYDATA,(WPARAM)0,(LPARAM)&CDS);
		free(pfoi2);//释放内存


		hr = CopyItems_old(pThis, punkItems, psiDestinationFolder);//系统默认文件复制操作
	} while (FALSE);

	if(!SUCCEEDED(hr))
	{
		FreeOPItem();
	}

	CoTaskMemFree(lpDst);
	return hr;
}

static HRESULT __stdcall MoveItems_new(IFileOperation* pThis,
									   IUnknown* punkItems,
									   IShellItem* psiDestinationFolder)
{
	OutputDebugStringA("8");
	LPWSTR lpDst = NULL;
	psiDestinationFolder->GetDisplayName(SIGDN_FILESYSPATH, &lpDst);
	HRESULT hr = S_FALSE;
	PFileOPItem pfoi = GetOPItem();

	do
	{
		OutputDebugStringA("MOVE文件，到：");
		OutputDebugStringW(lpDst);
		pfoi->srcCount = GetFilesFromDataObject(punkItems, &(pfoi->srcList));

		SetOPType(MOVE_FILE);
		wcscpy(pfoi->dstFolder, lpDst);


		//将copy数据合并到连续内存，方便传递
		COPYDATASTRUCT   CDS;
		void* pfoi2 =  malloc( sizeof(FileOPItem) + sizeof(WPATH) * (pfoi->srcCount) );
		//复制pfoi结构体到连续内存前部分
		memcpy(pfoi2, pfoi, sizeof(*pfoi)); 
		//复制pfoi->srcList到连续内存后半部分
		memcpy((BYTE*)pfoi2 + sizeof(FileOPItem), pfoi->srcList, sizeof(WPATH) * (pfoi->srcCount));   
		CDS.dwData = 0;   
		CDS.cbData = sizeof(FileOPItem) + sizeof(WPATH) * (pfoi->srcCount) ; //连续内存总长度
		CDS.lpData = pfoi2;//连续内存起始地址
		::SendMessage(HWND_BROADCAST, WM_COPYDATA,(WPARAM)0,(LPARAM)&CDS);
		free(pfoi2);//释放内存

		hr = MoveItems_old(pThis, punkItems, psiDestinationFolder);//系统默认文件移动操作
	} while (FALSE);

	if(!SUCCEEDED(hr))
	{
		FreeOPItem();
	}

	CoTaskMemFree(lpDst);
	return hr;
}

static HRESULT __stdcall PerformOperations_new(IFileOperation* pThis)
{
	OutputDebugStringA("9");
	HRESULT hr = PerformOperations_old(pThis);

	do
	{
		if(!SUCCEEDED(hr))
		{
			break;
		}

		OPTYPE type = GetOPType();
		if(COPY_FILE != type && MOVE_FILE != type)
		{
			break;
		}

		OutputDebugStringA("确认文件操作。");
		PFileOPItem pfoi = GetOPItem();
		for(unsigned int i = 0; i < pfoi->srcCount; ++i)
		{
			WPATH dstPath;
			wcscpy(dstPath, pfoi->dstFolder);
			PathAddBackslashW(dstPath);
			LPCWSTR tmp = (LPCWSTR)wcsrchr(pfoi->srcList[i], '\\');
			wcscat(dstPath, ++tmp);
			DWORD attrib = GetFileAttributesW(dstPath);
			if(INVALID_FILE_ATTRIBUTES == attrib)
			{
				continue;
			}
		}
	} while (FALSE);

	FreeOPItem();
	return hr;
}

BOOL WINAPI HookIFileOp()
{
	OutputDebugStringA("10");
	void* pInterface = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_SERVER, IID_IFileOperation, &pInterface);
	OutputDebugStringA("30");
	if(FAILED(hr))
	{
		OutputDebugStringA("40");
		return FALSE;
	}

	gs_opSlot = TlsAlloc();
	if(TLS_OUT_OF_INDEXES == gs_opSlot)
	{
		return FALSE;
	}

#define HOOK(a) \
	a##_old = (P##a)HookVtbl(pInterface, 0, a##_Index, (size_t)a##_new); \

	HOOK(CopyItems);
	HOOK(MoveItems);
	HOOK(PerformOperations);


#undef HOOK
	return TRUE;
}

BOOL WINAPI UnhookIFileOp()
{
	OutputDebugStringA("11");
	void* pInterface = NULL;
	if(FAILED(CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_SERVER, IID_IFileOperation, &pInterface)))
	{
		return FALSE;
	}

#define UNHOOK(a) \
	if(a##_old) \
	{ \
	HookVtbl(pInterface, 0, a##_Index, (size_t)a##_old); \
	} \

	OutputDebugStringA("解HOOK");
	UNHOOK(CopyItems);
	UNHOOK(MoveItems);
	UNHOOK(PerformOperations);


#undef UNHOOK

	if(TLS_OUT_OF_INDEXES != gs_opSlot)
	{
		TlsFree(gs_opSlot);
		gs_opSlot = TLS_OUT_OF_INDEXES;
	}

	return TRUE;
}

