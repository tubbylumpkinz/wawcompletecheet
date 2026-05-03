// src/hooks.h
#pragma once
#include <d3d9.h>
#include <windows.h>

using tPresent = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
    const RECT*, const RECT*, HWND, const RGNDATA*);
using tReset = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,
    D3DPRESENT_PARAMETERS*);

using tCClientFrame = void(__cdecl*)(int localClientNum);

inline tPresent      g_presentOrig        = nullptr;
inline tReset        g_resetOrig          = nullptr;
inline WNDPROC       g_origWndProc        = nullptr;
inline tCClientFrame g_clientFrameOrig    = nullptr;
inline bool          g_menuOpen           = false;

HRESULT STDMETHODCALLTYPE hkPresent(IDirect3DDevice9*,
    const RECT*, const RECT*, HWND, const RGNDATA*);
HRESULT STDMETHODCALLTYPE hkReset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
LRESULT CALLBACK          hkWndProc(HWND, UINT, WPARAM, LPARAM);
void    __cdecl           hkCClientFrame(int localClientNum);
