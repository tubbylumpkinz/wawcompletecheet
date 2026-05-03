// src/d3d_init.cpp
#include "d3d_init.h"
#include <d3d9.h>
#include <windows.h>

void** D3D9GetVTable() {
    HWND wnd = CreateWindowA("STATIC", "", WS_POPUP, 0, 0, 1, 1,
                              nullptr, nullptr, nullptr, nullptr);

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS pp{};
    pp.Windowed      = TRUE;
    pp.SwapEffect    = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = wnd;

    IDirect3DDevice9* dev = nullptr;
    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev);

    void** vtbl = *reinterpret_cast<void***>(dev);

    dev->Release();
    d3d->Release();
    DestroyWindow(wnd);

    return vtbl;
}
