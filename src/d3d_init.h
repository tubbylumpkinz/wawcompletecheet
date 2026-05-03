// src/d3d_init.h
#pragma once

// Creates a temporary D3D9 device purely to read its vtable,
// then releases it. Returns pointer to the shared vtable array.
// IDirect3DDevice9::Reset    = vtable[16]
// IDirect3DDevice9::Present  = vtable[17]
void** D3D9GetVTable();
