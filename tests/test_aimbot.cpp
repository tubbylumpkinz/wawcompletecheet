// tests/test_aimbot.cpp
#include <cassert>
#include <cmath>

struct Vec3 { float x, y, z; };

// Redeclare helpers inline so this compiles standalone (no DLL/offsets deps)
static constexpr float PI = 3.14159265f;
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

int main() {
    // AngleDelta: wraps correctly across 0/360 boundary
    assert(fabsf(AngleDelta(350.f, 10.f) - 20.f)  < 0.01f);
    assert(fabsf(AngleDelta(10.f, 350.f) + 20.f)   < 0.01f);
    // AngleDelta: same angle = 0
    assert(fabsf(AngleDelta(45.f, 45.f)) < 0.01f);

    // InFOV: target dead ahead is in FOV
    assert(InFOV(0.f, 0.f, 30.f) == true);
    // InFOV: 90 deg offset is outside 30 deg FOV
    assert(InFOV(90.f, 0.f, 30.f) == false);
    // InFOV: exactly at radius is outside (uses strict <)
    assert(InFOV(30.f, 0.f, 30.f) == false);
    assert(InFOV(29.9f, 0.f, 30.f) == true);

    // SmoothAngle: smooth=1 returns target exactly
    Vec3 r = SmoothAngle({0,0,0}, {10,20,0}, 1.f);
    assert(fabsf(r.x - 10.f) < 0.01f);
    assert(fabsf(r.y - 20.f) < 0.01f);

    // SmoothAngle: smooth=10 moves 1/10th of the way
    Vec3 r2 = SmoothAngle({0,0,0}, {10,0,0}, 10.f);
    assert(fabsf(r2.x - 1.f) < 0.01f);

    // CalcAngle: target directly above returns ~-90 pitch
    Vec3 a = CalcAngle({0,0,0}, {0,0,100});
    assert(fabsf(a.x + 90.f) < 1.f);

    // CalcAngle: target to the east (positive X) = 0 yaw
    Vec3 b = CalcAngle({0,0,0}, {100,0,0});
    assert(fabsf(b.y - 0.f) < 1.f);

    // CalcAngle: target to the north (positive Y) = 90 yaw
    Vec3 c = CalcAngle({0,0,0}, {0,100,0});
    assert(fabsf(c.y - 90.f) < 1.f);

    return 0;
}
