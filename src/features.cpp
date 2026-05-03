// src/features.cpp
#include "features.h"
#include "offsets.h"
#include "aimbot.h"
#include <cstring>
#include <cmath>

void ApplyFeatures() {
    // God Mode
    if (g_state.godMode && PLAYER_HEALTH_OFS != 0)
        Write<float>(PLAYER_HEALTH_OFS, 999.f);

    // Infinite Ammo (throttled to every 30 frames to reduce write pressure)
    if (g_state.infiniteAmmo && WEAPON_ARRAY_OFS != 0) {
        static int ammoTick = 0;
        if (++ammoTick >= 30) {
            Features::MaxAmmo();
            ammoTick = 0;
        }
    }

    // No Clip
    if (PLAYER_FLAGS_OFS != 0) {
        int32_t flags = Read<int32_t>(cCG + PLAYER_FLAGS_OFS);
        if (g_state.noClip)
            flags |= (1 << 9);
        else
            flags &= ~(1 << 9);
        Write<int32_t>(cCG + PLAYER_FLAGS_OFS, flags);
    }

    // Teleport: load saved position
    if (g_state.pendingLoadPos && g_state.hasSavedPos) {
        Write<float>(cCG + 0x28, g_state.savedPos.x);
        Write<float>(cCG + 0x2C, g_state.savedPos.y);
        Write<float>(cCG + 0x30, g_state.savedPos.z);
        g_state.pendingLoadPos = false;
    }

    // Freeze zombies (velocity zeroing — velocity offset TBD via CE)
    if (g_state.freezeZombies) {
        for (size_t i = 0; i < ENTITY_COUNT; i++) {
            uintptr_t ent = EntityList + (i * ENT_STRIDE);
            if (Read<float>(ent + ENT_HEALTH) > 0.f) {
                // Write<Vec3>(ent + ENT_VELOCITY_OFFSET, {0.f, 0.f, 0.f});
            }
        }
    }

    ApplyAimbot();
}

bool WorldToScreen(Vec3 world, float& sx, float& sy) {
    float* m = reinterpret_cast<float*>(ViewMatrix);

    float _x = m[0]*world.x + m[1]*world.y + m[2]*world.z + m[3];
    float _y = m[4]*world.x + m[5]*world.y + m[6]*world.z + m[7];
    float  w = m[12]*world.x + m[13]*world.y + m[14]*world.z + m[15];

    if (w < 0.01f) return false;

    int screenW = Read<int32_t>(cCGS + 0x8);
    int screenH = Read<int32_t>(cCGS + 0xC);

    sx = (screenW / 2.f) * (1.f + _x / w);
    sy = (screenH / 2.f) * (1.f - _y / w);
    return true;
}

namespace Features {
    void GiveAllWeapons() {
        if (WEAPON_ARRAY_OFS == 0) return;
        // Weapon indices vary — verify in CE. Placeholder: no-op until WEAPON_ARRAY filled.
    }

    void MaxAmmo() {
        if (WEAPON_ARRAY_OFS == 0) return;
        for (int i = 0; i < 10; i++) {
            uintptr_t slot = Read<uintptr_t>(WEAPON_ARRAY_OFS) + (i * 0x10);
            if (slot) Write<int32_t>(slot + 0x4, 9999);
        }
    }

    void MaxPoints() {
        if (POINTS_ADDR != 0) Write<int32_t>(POINTS_ADDR, 999999);
    }

    void BuyAllDoors() {
        MaxPoints();
    }

    void AllPerks() {
        // Write<int32_t>(cCG + PERK_FLAGS_OFFSET, 0xF); // locate offset via CE
    }

    void KillAllZombies() {
        for (size_t i = 0; i < ENTITY_COUNT; i++) {
            uintptr_t ent = EntityList + (i * ENT_STRIDE);
            if (Read<float>(ent + ENT_HEALTH) > 0.f)
                Write<float>(ent + ENT_HEALTH, 0.f);
        }
    }

    void SetRound(int round) {
        if (ROUND_ADDR != 0) Write<int32_t>(ROUND_ADDR, round);
    }

    void TeleportToSpawn() {
        // Spawn coords vary by map — implement once PLAYER_POS_OFFSET confirmed
    }
}
