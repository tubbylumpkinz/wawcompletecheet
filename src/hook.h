// src/hook.h
#pragma once
#include <windows.h>
#include <cstddef>

// Patches one slot in a COM vtable. Returns the original function pointer.
inline void* VTableHook(void** vtable, size_t idx, void* fn) {
    DWORD old;
    VirtualProtect(&vtable[idx], sizeof(void*), PAGE_EXECUTE_READWRITE, &old);
    void* orig = vtable[idx];
    vtable[idx] = fn;
    VirtualProtect(&vtable[idx], sizeof(void*), old, &old);
    return orig;
}
