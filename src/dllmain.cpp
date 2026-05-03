// src/dllmain.cpp
#include <windows.h>
#include <cstring>
#include "d3d_init.h"
#include "hook.h"
#include "hooks.h"
#include "offsets.h"

static DWORD WINAPI HookThread(LPVOID) {
    // Wait until the game has loaded d3d9.dll
    while (!GetModuleHandleA("d3d9.dll"))
        Sleep(100);
    Sleep(500); // brief delay for game device init

    void** vtbl = D3D9GetVTable();
    g_presentOrig = (tPresent)VTableHook(vtbl, 17, (void*)hkPresent);
    g_resetOrig   = (tReset)  VTableHook(vtbl, 16, (void*)hkReset);
    OutputDebugStringA("[waw_menu] D3D9 Hooks applied.");

    HWND wnd = FindWindowA("CoD-WaW", nullptr);
    if (!wnd) {
        wnd = FindWindowA(nullptr, "Call of Duty\xAE"); // \xAE is the ® symbol in ANSI
    }

    if (wnd) {
        g_origWndProc = (WNDPROC)SetWindowLongPtrA(wnd, GWLP_WNDPROC,
                                                    (LONG_PTR)hkWndProc);
        OutputDebugStringA("[waw_menu] WndProc hooked successfully.");
    } else {
        OutputDebugStringA("[waw_menu] ERROR: Could not find CoD window by class or title.");
    }

    /* 
    // Patch CClientFrame with a 5-byte relative JMP for silent aim
    // TODO: Implement a proper trampoline or safer hooking method to avoid crashes
    if (CClientFrame != 0) {
        BYTE jmp[5] = { 0xE9 };
        uintptr_t rel = (uintptr_t)hkCClientFrame - CClientFrame - 5;
        memcpy(jmp + 1, &rel, 4);
        DWORD old;
        VirtualProtect((void*)CClientFrame, 5, PAGE_EXECUTE_READWRITE, &old);
        memcpy((void*)CClientFrame, jmp, 5);
        VirtualProtect((void*)CClientFrame, 5, old, &old);
    }
    */

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        CloseHandle(CreateThread(nullptr, 0, HookThread, nullptr, 0, nullptr));
    }
    return TRUE;
}
