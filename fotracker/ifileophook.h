/*****************************************************************************\
 * @file   : IFileOPHook.h                                                   *
 * @author : ����                                                            *
 * @date   : 2011.10.27                                                      *
 * @note   : VISTA �� WIN7 ���ļ���������ʹ�� IFileOperation ��������XP����  *
 *           ��ϵͳҪ���ļ�������ؾͱ��� HOOK IFileOperation ��             *
\*****************************************************************************/
#ifndef _H_IFILEOP_HOOK
#define _H_IFILEOP_HOOK

//#define WM_MYFILEOP (WM_USER+379)
#define FILE_OP	379
#define DLL_HOOKED 184
#define DLL_UNHOOK 514
static UINT uMsgMyFileOp = ::RegisterWindowMessage(L"uMsgMyFileOp");



#ifdef __cplusplus
extern "C"
{
#endif

#include <windows.h>

    typedef WCHAR WPATH[MAX_PATH];

    enum OPTYPE
    {
        UNDEF = 0,
        MOVE_FILE,
        COPY_FILE,
        DELETE_FILE,
        NEW_FILE,
		RENAME_FILE
    };

    typedef struct _FileOPItem
    {
        OPTYPE opType;
        unsigned int srcCount;
        WPATH* srcList;
        WPATH dstFolder;
    } FileOPItem, *PFileOPItem;

    BOOL WINAPI HookIFileOp();
    BOOL WINAPI UnhookIFileOp();
	

#ifdef __cplusplus
}
#endif

#endif
