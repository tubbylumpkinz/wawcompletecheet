// src/offsets.h
#pragma once
#include <cstdint>
#include <cstddef>

// --- Hooks ---
constexpr uintptr_t CClientFrame      = 0x0043A170;

// --- Static class pointers ---
constexpr uintptr_t cClientInfo       = 0x35B7E48;
constexpr uintptr_t cRefDef           = 0x3520338;
constexpr uintptr_t PLAYER_VIEW_OFS   = 0x1C;      // Vec3 view origin in cRefDef
constexpr uintptr_t cCG               = 0x34732B8;
constexpr uintptr_t cCGS              = 0x3466578;
constexpr uintptr_t cEntity           = 0x35D39F0;  // entity array base

// --- Drawing / font ---
constexpr uintptr_t cDrawStringEngine = 0x006F5F10;
constexpr uintptr_t cDrawStretchPic   = 0x0044EE30;
constexpr uintptr_t cRegisterShader   = 0x006E9B80;
constexpr uintptr_t cRegisterFont     = 0x006E8CE0;

// --- ViewAngles (static floats) ---
constexpr uintptr_t ViewAngleX        = 0x0307D6D4;  // pitch
constexpr uintptr_t ViewAngleY        = 0x0307D6D0;  // yaw

// --- Extras ---
constexpr uintptr_t cMedia            = 0x020A10D0;
constexpr uintptr_t cFonts            = 0x020A10F8;

// --- Static Addresses (SP/Zombies) ---
constexpr uintptr_t POINTS_ADDR       = 0x018EF124;  // Static pointer to points
constexpr uintptr_t ROUND_ADDR        = 0x018EE644;  // Static pointer to round (example)

// --- Struct Offsets (into cCG) ---
constexpr uintptr_t PLAYER_HEALTH_OFS = 0x176C8B8;  // Verify if this is an offset or static. 
constexpr uintptr_t PLAYER_FLAGS_OFS  = 0x0;        // int32  — move flags (bit 9 = noclip)
constexpr uintptr_t WEAPON_ARRAY_OFS  = 0x0;        // ptr    — weapon slot array base

// --- Entity list (SP/Zombies standard) ---
constexpr uintptr_t EntityList        = 0x35D39F0;  // Unified base
constexpr uintptr_t ViewMatrix        = 0x008E870C; // static base
constexpr size_t    ENTITY_COUNT      = 64;
constexpr size_t    ENT_STRIDE        = 0x81B;      // centity_t size for SP

// Entity struct field offsets (verified for 0x81B stride)
constexpr uintptr_t ENT_HEALTH        = 0x1C8;     // float
constexpr uintptr_t ENT_FEET          = 0x18;      // Vec3
constexpr uintptr_t ENT_HEAD          = 0x154;     // Vec3

// Legacy cCEntity_t field offsets (kept for reference)
constexpr uintptr_t ENT_VALID         = 0x0002;
constexpr uintptr_t ENT_ORIGIN        = 0x001F;
constexpr uintptr_t ENT_TYPE          = 0x00CF;
constexpr uintptr_t ENT_ALIVE         = 0x020B;
constexpr uintptr_t ENT_NEWORIGIN     = 0x01EB;
