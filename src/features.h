// src/features.h
#pragma once
#include <cstdint>
#include "imgui/imgui.h"

// Direct memory access helpers (we're in-process, no RPM needed)
template<typename T>
inline T Read(uintptr_t addr) {
    return *reinterpret_cast<T*>(addr);
}
template<typename T>
inline void Write(uintptr_t addr, T val) {
    *reinterpret_cast<T*>(addr) = val;
}

struct Vec3 { float x, y, z; };

struct FeatureState {
    // Player
    bool  godMode       = false;
    bool  infiniteAmmo  = false;
    bool  noClip        = false;
    float moveSpeed     = 1.0f;

    // Weapons
    bool  rapidFire     = false;
    bool  noRecoil      = false;
    bool  noSpread      = false;

    // Zombies
    bool  freezeZombies   = false;
    float zombieSpeed     = 1.f;
    float zombieHealthMul = 1.f;

    // Round
    bool  instantRoundEnd = false;

    // Teleport
    Vec3  savedPos            = {};
    bool  hasSavedPos         = false;
    bool  pendingLoadPos      = false;
    bool  pendingTeleportSpawn = false;

    // Visuals
    bool    espEnabled      = false;
    ImVec4  espColor        = {1.f, 0.f, 0.f, 1.f};
    bool    customCrosshair = false;
    float   fovOverride     = 65.f;
    bool    removeFog       = false;

    // Aimbot
    bool  aimbotEnabled  = false;
    bool  silentAim      = false;
    int   boneTarget     = 0;    // 0=Head 1=Neck 2=Chest 3=Pelvis
    float aimFOV         = 90.f;
    float smoothness     = 5.f;
    bool  aimPrediction  = false;
    bool  visibleOnly    = false;
    bool  drawFOVCircle  = false;
};

inline FeatureState g_state;

namespace Features {
    void GiveAllWeapons();
    void MaxAmmo();
    void MaxPoints();
    void BuyAllDoors();
    void AllPerks();
    void KillAllZombies();
    void SetRound(int round);
    void TeleportToSpawn();
}

// Projects world position to screen. Returns false if behind camera.
bool WorldToScreen(Vec3 world, float& sx, float& sy);

// Called once per frame from hkPresent (before ImGui::NewFrame)
void ApplyFeatures();
