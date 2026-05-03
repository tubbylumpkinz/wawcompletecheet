// src/hooks.cpp
#include "hooks.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "features.h"
#include "aimbot.h"
#include "menu.h"
#include "offsets.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND, UINT, WPARAM, LPARAM);

HRESULT STDMETHODCALLTYPE hkPresent(IDirect3DDevice9* dev,
    const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty)
{
    static bool init = false;
    if (!init) {
        OutputDebugStringA("[waw_menu] Initializing ImGui...");
        HWND wnd_handle = FindWindowA("Call of Duty\xAE", nullptr);
        if (!wnd_handle) wnd_handle = FindWindowA(nullptr, "Call of Duty\xAE");
        
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(wnd_handle);
        ImGui_ImplDX9_Init(dev);
        Menu::ApplyTheme();
        init = true;
        OutputDebugStringA("[waw_menu] ImGui initialized.");
    }

    ApplyFeatures();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (g_menuOpen)
        Menu::Draw();

    // Always-on overlays
    if (g_state.customCrosshair) {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImGuiIO& io = ImGui::GetIO();
        float cx = io.DisplaySize.x / 2.f, cy = io.DisplaySize.y / 2.f;
        dl->AddLine({cx-10,cy}, {cx+10,cy}, IM_COL32(255,255,255,200), 1.5f);
        dl->AddLine({cx,cy-10}, {cx,cy+10}, IM_COL32(255,255,255,200), 1.5f);
    }
    Menu::DrawESP();
    if (g_state.drawFOVCircle) {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImGuiIO& io = ImGui::GetIO();
        float cx = io.DisplaySize.x / 2.f, cy = io.DisplaySize.y / 2.f;
        float radius = (g_state.aimFOV / 180.f) * (io.DisplaySize.y / 2.f);
        dl->AddCircle({cx, cy}, radius, IM_COL32(123, 104, 238, 180), 64, 1.5f);
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    return g_presentOrig(dev, src, dst, wnd, dirty);
}

HRESULT STDMETHODCALLTYPE hkReset(IDirect3DDevice9* dev,
    D3DPRESENT_PARAMETERS* pp)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_resetOrig(dev, pp);
    ImGui_ImplDX9_CreateDeviceObjects();
    return hr;
}

LRESULT CALLBACK hkWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_KEYDOWN && wp == VK_INSERT) {
        g_menuOpen = !g_menuOpen;
        OutputDebugStringA(g_menuOpen ? "[waw_menu] Menu Opened" : "[waw_menu] Menu Closed");
        ImGui::GetIO().MouseDrawCursor = g_menuOpen;
        ShowCursor(g_menuOpen);
    }
    if (g_menuOpen && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
        return TRUE;
    return CallWindowProcA(g_origWndProc, hwnd, msg, wp, lp);
}

void __cdecl hkCClientFrame(int localClientNum) {
    static float savedX = 0.f, savedY = 0.f;
    if (g_state.aimbotEnabled && g_state.silentAim) {
        savedX = Read<float>(ViewAngleX);
        savedY = Read<float>(ViewAngleY);
        ApplySilentAim();
    }
    if (g_clientFrameOrig)
        g_clientFrameOrig(localClientNum);
    if (g_state.aimbotEnabled && g_state.silentAim) {
        Write<float>(ViewAngleX, savedX);
        Write<float>(ViewAngleY, savedY);
    }
}
