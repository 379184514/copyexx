/*****************************************************************************\
 * @file   : main.cpp                                                        *
 * @author : 吴永                                                            *
 * @date   : 2011.10.20                                                      *
 * @note   : 承 vdkey.dll 的经验，为了尽量减少模块，此模块分成几种工作模式： *
 *           1. 编辑器模式，此时只需要加载认证库编辑器而不需要HOOK模块       *
 *           2. 注入模式，此时需要加载认证库编辑器和HOOK模块                 *
 *           3. 策略管理模式，此时要向服务器方请求本地策略，也要编辑本地策略 *
 *              ，需要加载认证库编辑器。由于这一般是客户端行为，所以也应加载 *
 *              HOOK 模块。在本 DLL 进入时加载模块，但下载等行为提供另外的接 *
 *              口来进入工作状态。                                           *
 *           控制台会使用编辑器模式，加载本模块后需要调用编辑器模块定义的函  *
 *           数开始编辑。室                                                  *
 *           注入模式调用 InstallGlobalHook，退出注入调用 UninstallGlobalHook*
 *           策略管理模式调用 StartWorkThread，退出调用 EndWorkThread        *
 *           以上函数都将导出，可以多个模式使用该模块，但合理的用法是控制台  *
 *           只使用编辑器模式(控制台和版本将和客户端的版本不太一样，控制台的 *
 *           只加载编辑器的动态库，即是说即使控制台版本的程序调用函数试图实  *
 *           现注入功能，也将因为缺少动态库而失败)，客户端则同时使用注入模式 *
 *           和策略管理模式。                                                *
 *           注意，如果是 rundll32.exe 来做为注入器，应先进入策略管理模式，  *
 *           再进入注入模式。因为对 rundll32 的过早退出的额外处理在注入模式  *
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

HMODULE g_hThisModule = 0;                  // 本模块句柄
static BOOL gs_bHooked = FALSE;             // 是否HOOK



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
        OutputDebugStringA("00HOOK explorer.exe 的 IFileOperation");
        gs_bHooked = HookIFileOp();
		//::SendMessage(HWND_BROADCAST,uMsgMyFileOp /*WM_MYFILEOP*/, (WPARAM)DLL_HOOKED,(LPARAM)0);
		::PostMessage(HWND_BROADCAST,uMsgMyFileOp /*WM_MYFILEOP*/, (WPARAM)DLL_HOOKED,(LPARAM)0);
    }
}

void OnFreeDll()
{
    if(gs_bHooked)
    {
        OutputDebugStringA("00解 explorer.exe 的HOOK");
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

// 进入注入模式
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
