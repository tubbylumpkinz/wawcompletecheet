// src/aimbot.cpp
#include "aimbot.h"
#include "offsets.h"
#include <cmath>

static constexpr float PI = 3.14159265f;
static inline float ToRad(float deg) { return deg * PI / 180.f; }
static inline float ToDeg(float rad) { return rad * 180.f / PI; }

float AngleDelta(float a, float b) {
    float d = fmodf(b - a + 180.f, 360.f) - 180.f;
    if (d < -180.f) d += 360.f;
    return d;
}

bool InFOV(float deltaYaw, float deltaPitch, float fovRadius) {
    float dist = sqrtf(deltaYaw * deltaYaw + deltaPitch * deltaPitch);
    return dist < fovRadius;
}

Vec3 SmoothAngle(Vec3 current, Vec3 target, float smooth) {
    return {
        current.x + (target.x - current.x) / smooth,
        current.y + (target.y - current.y) / smooth,
        0.f
    };
}

Vec3 CalcAngle(Vec3 from, Vec3 to) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float dz = to.z - from.z;
    float dist2d = sqrtf(dx * dx + dy * dy);
    float pitch = ToDeg(atan2f(-dz, dist2d));
    float yaw   = ToDeg(atan2f(dy, dx));
    return {pitch, yaw, 0.f};
}

static Vec3 GetEntityHeadPos(uintptr_t ent) {
    return {
        Read<float>(ent + ENT_HEAD),
        Read<float>(ent + ENT_HEAD + 4),
        Read<float>(ent + ENT_HEAD + 8)
    };
}

int GetBestTarget() {
    float curYaw   = Read<float>(ViewAngleY);
    float curPitch = Read<float>(ViewAngleX);
    int   best     = -1;
    float bestDist = g_state.aimFOV;

    Vec3 myPos; 
    myPos.x = Read<float>(cRefDef + PLAYER_VIEW_OFS);
    myPos.y = Read<float>(cRefDef + PLAYER_VIEW_OFS + 4);
    myPos.z = Read<float>(cRefDef + PLAYER_VIEW_OFS + 8);

    for (size_t i = 0; i < ENTITY_COUNT; i++) {
        uintptr_t ent = EntityList + (i * ENT_STRIDE);

        float hp = Read<float>(ent + ENT_HEALTH);
        if (hp <= 0.f) continue;

        Vec3  angle  = CalcAngle(myPos, GetEntityHeadPos(ent));
        float dYaw   = AngleDelta(curYaw,   angle.y);
        float dPitch = AngleDelta(curPitch, angle.x);

        if (!InFOV(dYaw, dPitch, g_state.aimFOV)) continue;

        float dist = sqrtf(dYaw * dYaw + dPitch * dPitch);
        if (dist < bestDist) {
            bestDist = dist;
            best     = (int)i;
        }
    }
    return best;
}

void ApplyAimbot() {
    if (!g_state.aimbotEnabled || g_state.silentAim) return;

    int target = GetBestTarget();
    if (target < 0) return;

    uintptr_t ent  = EntityList + (target * ENT_STRIDE);
    
    Vec3 myPos;
    myPos.x = Read<float>(cRefDef + PLAYER_VIEW_OFS);
    myPos.y = Read<float>(cRefDef + PLAYER_VIEW_OFS + 4);
    myPos.z = Read<float>(cRefDef + PLAYER_VIEW_OFS + 8);

    Vec3 targetAngle = CalcAngle(myPos, GetEntityHeadPos(ent));
    Vec3 current     = { Read<float>(ViewAngleX), Read<float>(ViewAngleY), 0.f };
    Vec3 smoothed    = SmoothAngle(current, targetAngle, g_state.smoothness);

    Write<float>(ViewAngleX, smoothed.x);
    Write<float>(ViewAngleY, smoothed.y);
}

void ApplySilentAim() {
    if (!g_state.aimbotEnabled || !g_state.silentAim) return;

    int target = GetBestTarget();
    if (target < 0) return;

    uintptr_t ent    = EntityList + (target * ENT_STRIDE);
    
    Vec3 myPos;
    myPos.x = Read<float>(cRefDef + PLAYER_VIEW_OFS);
    myPos.y = Read<float>(cRefDef + PLAYER_VIEW_OFS + 4);
    myPos.z = Read<float>(cRefDef + PLAYER_VIEW_OFS + 8);

    Vec3 targetAngle = CalcAngle(myPos, GetEntityHeadPos(ent));

    Write<float>(ViewAngleX, targetAngle.x);
    Write<float>(ViewAngleY, targetAngle.y);
}
