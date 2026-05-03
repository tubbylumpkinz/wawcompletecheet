#pragma once
#include "features.h"

float AngleDelta(float a, float b);
bool  InFOV(float deltaYaw, float deltaPitch, float fovRadius);
Vec3  SmoothAngle(Vec3 current, Vec3 target, float smooth);
Vec3  CalcAngle(Vec3 from, Vec3 to);

// Returns best zombie entity index in FOV, or -1 if none
int   GetBestTarget();

// Writes aim angles each frame (standard aimbot) — called from ApplyFeatures
void  ApplyAimbot();

// Silent aim: redirect shot angles, then restore — called from CClientFrame hook
void  ApplySilentAim();
