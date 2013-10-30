/*****************************************************************************\
 * @file   : main.cpp                                                        *
 * @author : ����                                                            *
 * @date   : 2011.10.20                                                      *
 * @note   : �� vdkey.dll �ľ��飬Ϊ�˾�������ģ�飬��ģ��ֳɼ��ֹ���ģʽ�� *
 *           1. �༭��ģʽ����ʱֻ��Ҫ������֤��༭��������ҪHOOKģ��       *
 *           2. ע��ģʽ����ʱ��Ҫ������֤��༭����HOOKģ��                 *
 *           3. ���Թ���ģʽ����ʱҪ������������󱾵ز��ԣ�ҲҪ�༭���ز��� *
 *              ����Ҫ������֤��༭����������һ���ǿͻ�����Ϊ������ҲӦ���� *
 *              HOOK ģ�顣�ڱ� DLL ����ʱ����ģ�飬�����ص���Ϊ�ṩ����Ľ� *
 *              �������빤��״̬��                                           *
 *           ����̨��ʹ�ñ༭��ģʽ�����ر�ģ�����Ҫ���ñ༭��ģ�鶨��ĺ�  *
 *           ����ʼ�༭����                                                  *
 *           ע��ģʽ���� InstallGlobalHook���˳�ע����� UninstallGlobalHook*
 *           ���Թ���ģʽ���� StartWorkThread���˳����� EndWorkThread        *
 *           ���Ϻ����������������Զ��ģʽʹ�ø�ģ�飬��������÷��ǿ���̨  *
 *           ֻʹ�ñ༭��ģʽ(����̨�Ͱ汾���Ϳͻ��˵İ汾��̫һ��������̨�� *
 *           ֻ���ر༭���Ķ�̬�⣬����˵��ʹ����̨�汾�ĳ�����ú�����ͼʵ  *
 *           ��ע�빦�ܣ�Ҳ����Ϊȱ�ٶ�̬���ʧ��)���ͻ�����ͬʱʹ��ע��ģʽ *
 *           �Ͳ��Թ���ģʽ��                                                *
 *           ע�⣬����� rundll32.exe ����Ϊע������Ӧ�Ƚ�����Թ���ģʽ��  *
 *           �ٽ���ע��ģʽ����Ϊ�� rundll32 �Ĺ����˳��Ķ��⴦����ע��ģʽ  *
\*****************************************************************************/
#include <windows.h>
#include "ifileophook.h"


//#define OutputDebugStringA(ss) MessageBoxA(NULL,ss,"Hooked - ANSI",MB_OK);
//#define OutputDebugStringW(ss) MessageBoxW(NULL,ss,L"Hooked - Unicode",MB_OK);

#define IEQUALW(str1, str2) (str1 && str2 && (0 == wcsicmp(str1, str2)))

#pragma warning(disable:4996)
#pragma comment(linker, "/Section:.shared,RWS")

#pragma data_seg(".shared")
HHOOK g_oldHook = NULL;                     // CallNextHook
#pragma  data_seg()

HMODULE g_hThisModule = 0;                  // ��ģ����
static BOOL gs_bHooked = FALSE;             // �Ƿ�HOOK



BOOL WINAPI IsProcessW(LPCWSTR procName)
{
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(0, path, MAX_PATH);
    wchar_t* tmp = (wchar_t*)wcsrchr(path, L'\\');
    if(!tmp)
    {
        return FALSE;
    }
    tmp++;
    return IEQUALW(tmp, procName);
}

void OnLoadDll()
{
    if(IsProcessW(L"explorer.exe"))
    {
        OutputDebugStringA("00HOOK explorer.exe �� IFileOperation");
        gs_bHooked = HookIFileOp();
		//::SendMessage(HWND_BROADCAST,uMsgMyFileOp /*WM_MYFILEOP*/, (WPARAM)DLL_HOOKED,(LPARAM)0);
		::PostMessage(HWND_BROADCAST,uMsgMyFileOp /*WM_MYFILEOP*/, (WPARAM)DLL_HOOKED,(LPARAM)0);
    }
}

void OnFreeDll()
{
    if(gs_bHooked)
    {
        OutputDebugStringA("00�� explorer.exe ��HOOK");
        UnhookIFileOp();
		//::SendMessage(HWND_BROADCAST, uMsgMyFileOp, (WPARAM)DLL_UNHOOK,(LPARAM)0);	
    }
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD dwReason, LPVOID reserved)
{
    g_hThisModule = (HMODULE)handle;
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(g_hThisModule);
        OnLoadDll();
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        OnFreeDll();
        break;
    }
    }

    return TRUE;
}

LRESULT CALLBACK GlobalCBTHook(int nCode, WPARAM w, LPARAM l)
{
    if(g_oldHook)
    {
        return CallNextHookEx(g_oldHook, nCode, w, l);
    }
    return 0;
}

BOOL WINAPI InstallGlobalHook()
{
    g_oldHook = SetWindowsHookEx(WH_CBT, GlobalCBTHook, g_hThisModule, 0);

    return TRUE;
}

void WINAPI UninstallGlobalHook()
{
    if(g_oldHook)
    {
        UnhookWindowsHookEx(g_oldHook);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM w, LPARAM l)
{
    if(WM_DESTROY == msg)
    {
        PostQuitMessage(0);
    }

    return DefWindowProcA(hWnd, msg, w, l);
}

void WINAPI EnterMessageLoop()
{
    do
    {
        WNDCLASSEXA wcex = {sizeof(wcex)};

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = (WNDPROC)WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = g_hThisModule;
        wcex.hIcon = LoadIcon(NULL, IDI_INFORMATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcex.lpszClassName = "proclimitwnd";

        if(!RegisterClassExA(&wcex))
        {
            break;
        }

        HWND hWnd = CreateWindowExA(0, "proclimitwnd", NULL, WS_OVERLAPPEDWINDOW,
                                    0, 0, 50, 50, NULL, NULL, g_hThisModule, NULL);
        if(!IsWindow(hWnd))
        {
            break;
        }

        UpdateWindow(hWnd);
        MSG msg;

        while(GetMessage(&msg, hWnd, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    } while (FALSE);
}

// ����ע��ģʽ
void WINAPI EnterInjectMode()
{
    if(!InstallGlobalHook())
    {
        return;
    }

    if(IsProcessW(L"rundll32.exe"))
    {
        EnterMessageLoop();
    }
}
