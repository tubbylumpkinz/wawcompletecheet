// src/menu.cpp
#include "menu.h"
#include "features.h"
#include "offsets.h"
#include "imgui/imgui.h"

static void DrawPlayerTab();
static void DrawWeaponsTab();
static void DrawEconomyTab();
static void DrawZombiesTab();
static void DrawRoundTab();
static void DrawTeleportTab();
static void DrawVisualsTab();
static void DrawAimbotTab();

void Menu::ApplyTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    ImVec4* c = s.Colors;

    s.WindowRounding    = 6.f;
    s.FrameRounding     = 3.f;
    s.ScrollbarRounding = 3.f;
    s.TabRounding       = 3.f;
    s.WindowBorderSize  = 1.f;
    s.FrameBorderSize   = 0.f;
    s.ItemSpacing       = {8, 6};
    s.WindowPadding     = {12, 10};

    c[ImGuiCol_WindowBg]         = {0.051f, 0.051f, 0.122f, 1.f};
    c[ImGuiCol_TitleBg]          = {0.059f, 0.059f, 0.145f, 1.f};
    c[ImGuiCol_TitleBgActive]    = {0.059f, 0.059f, 0.145f, 1.f};
    c[ImGuiCol_Tab]              = {0.039f, 0.039f, 0.102f, 1.f};
    c[ImGuiCol_TabActive]        = {0.482f, 0.408f, 0.933f, 1.f};
    c[ImGuiCol_TabHovered]       = {0.380f, 0.318f, 0.750f, 1.f};
    c[ImGuiCol_FrameBg]          = {0.118f, 0.118f, 0.220f, 1.f};
    c[ImGuiCol_FrameBgHovered]   = {0.157f, 0.157f, 0.275f, 1.f};
    c[ImGuiCol_SliderGrab]       = {0.482f, 0.408f, 0.933f, 1.f};
    c[ImGuiCol_SliderGrabActive] = {0.608f, 0.533f, 1.000f, 1.f};
    c[ImGuiCol_CheckMark]        = {0.482f, 0.408f, 0.933f, 1.f};
    c[ImGuiCol_Button]           = {0.227f, 0.188f, 0.667f, 1.f};
    c[ImGuiCol_ButtonHovered]    = {0.380f, 0.318f, 0.800f, 1.f};
    c[ImGuiCol_ButtonActive]     = {0.482f, 0.408f, 0.933f, 1.f};
    c[ImGuiCol_Header]           = {0.200f, 0.188f, 0.400f, 1.f};
    c[ImGuiCol_HeaderHovered]    = {0.300f, 0.280f, 0.550f, 1.f};
    c[ImGuiCol_Separator]        = {0.165f, 0.165f, 0.290f, 1.f};
    c[ImGuiCol_Text]             = {0.753f, 0.753f, 0.941f, 1.f};
    c[ImGuiCol_TextDisabled]     = {0.439f, 0.439f, 0.627f, 1.f};
    c[ImGuiCol_Border]           = {0.165f, 0.165f, 0.314f, 1.f};
    c[ImGuiCol_PopupBg]          = {0.063f, 0.063f, 0.133f, 0.98f};
}

void Menu::Draw() {
    ImGui::SetNextWindowSize({480, 340}, ImGuiCond_Once);
    ImGui::SetNextWindowPos({100, 100}, ImGuiCond_Once);

    ImGui::Begin("@tubbylumpkinz lolz",
        nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    if (ImGui::BeginTabBar("MainTabs")) {
        if (ImGui::BeginTabItem("Player"))   { DrawPlayerTab();   ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Weapons"))  { DrawWeaponsTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Economy"))  { DrawEconomyTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Zombies"))  { DrawZombiesTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Round"))    { DrawRoundTab();    ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Teleport")) { DrawTeleportTab(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Visuals"))  { DrawVisualsTab();  ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("Aimbot"))   { DrawAimbotTab();   ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }

    ImGui::Separator();
    if (ROUND_ADDR != 0 && POINTS_ADDR != 0) {
        ImGui::TextDisabled("Round %d  |  %d pts  |  v1.0",
            Read<int32_t>(ROUND_ADDR), Read<int32_t>(POINTS_ADDR));
    } else {
        ImGui::TextDisabled("v1.0  —  fill ROUND_ADDR + POINTS_ADDR in offsets.h");
    }

    ImGui::End();
}

void Menu::DrawESP() {
    if (!g_state.espEnabled) return;

    ImDrawList* dl  = ImGui::GetBackgroundDrawList();
    ImU32       col = ImGui::ColorConvertFloat4ToU32(g_state.espColor);

    for (size_t i = 0; i < ENTITY_COUNT; i++) {
        uintptr_t ent = EntityList + (i * ENT_STRIDE);

        float hp = Read<float>(ent + ENT_HEALTH);
        if (hp <= 0.f) continue;

        Vec3 feet, head;
        feet.x = Read<float>(ent + ENT_FEET);
        feet.y = Read<float>(ent + ENT_FEET + 4);
        feet.z = Read<float>(ent + ENT_FEET + 8);
        head.x = Read<float>(ent + ENT_HEAD);
        head.y = Read<float>(ent + ENT_HEAD + 4);
        head.z = Read<float>(ent + ENT_HEAD + 8);

        float fx, fy, hx, hy;
        if (!WorldToScreen(feet, fx, fy)) continue;
        if (!WorldToScreen(head, hx, hy)) continue;

        float height = fy - hy;
        if (height < 2.f) continue; // degenerate / too far

        float width = height / 4.f;

        // Bounding box + head dot
        dl->AddRect({hx - width, hy}, {hx + width, fy}, col, 0.f, 0, 1.5f);
        dl->AddCircleFilled({hx, hy - 3.f}, 3.f, col);
    }
}

static void DrawPlayerTab() {
    ImGui::TextDisabled("PLAYER");
    ImGui::Separator();
    ImGui::Checkbox("God Mode",      &g_state.godMode);
    ImGui::Checkbox("Infinite Ammo", &g_state.infiniteAmmo);
    ImGui::Checkbox("No Clip",       &g_state.noClip);
    ImGui::SliderFloat("Move Speed", &g_state.moveSpeed, 0.1f, 5.0f, "%.1fx");
}

static void DrawWeaponsTab() {
    ImGui::TextDisabled("WEAPONS");
    ImGui::Separator();
    if (ImGui::Button("Give All Weapons", {-1, 0}))
        Features::GiveAllWeapons();
    ImGui::Spacing();
    ImGui::Checkbox("Rapid Fire", &g_state.rapidFire);
    ImGui::Checkbox("No Recoil",  &g_state.noRecoil);
    ImGui::Checkbox("No Spread",  &g_state.noSpread);
    ImGui::Spacing();
    if (ImGui::Button("Max Ammo", {-1, 0}))
        Features::MaxAmmo();
}

static void DrawEconomyTab() {
    ImGui::TextDisabled("ECONOMY");
    ImGui::Separator();
    if (ImGui::Button("Max Points", {-1, 0}))
        Features::MaxPoints();
    ImGui::Spacing();
    static int pointsInput = 1000;
    ImGui::SetNextItemWidth(120);
    ImGui::InputInt("##pts", &pointsInput);
    ImGui::SameLine();
    if (ImGui::Button("Set Points"))
        if (POINTS_ADDR != 0) Write<int32_t>(POINTS_ADDR, pointsInput);
    ImGui::Spacing();
    if (ImGui::Button("Buy All Doors", {-1, 0})) Features::BuyAllDoors();
    if (ImGui::Button("All Perks",     {-1, 0})) Features::AllPerks();
}

static void DrawZombiesTab() {
    ImGui::TextDisabled("ZOMBIES");
    ImGui::Separator();
    if (ImGui::Button("Kill All Zombies", {-1, 0}))
        Features::KillAllZombies();
    ImGui::Spacing();
    ImGui::Checkbox("Freeze Zombies",   &g_state.freezeZombies);
    ImGui::SliderFloat("Zombie Speed",  &g_state.zombieSpeed,     0.f, 3.f, "%.2f");
    ImGui::SliderFloat("Zombie Health", &g_state.zombieHealthMul, 0.1f, 5.f, "%.1fx");
}
static void DrawRoundTab() {
    ImGui::TextDisabled("ROUND");
    ImGui::Separator();
    if (ROUND_ADDR != 0)
        ImGui::Text("Current Round: %d", Read<int32_t>(ROUND_ADDR));
    else
        ImGui::TextDisabled("Current Round: (fill ROUND_ADDR offset)");
    ImGui::Spacing();
    static int roundInput = 10;
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("##rnd", &roundInput);
    if (roundInput < 1) roundInput = 1;
    ImGui::SameLine();
    if (ImGui::Button("Set Round"))
        Features::SetRound(roundInput);
    ImGui::Spacing();
    if (ImGui::Button("Skip to Next Round", {-1, 0}))
        if (ROUND_ADDR != 0) Features::SetRound(Read<int32_t>(ROUND_ADDR) + 1);
    ImGui::Checkbox("Instant Round End", &g_state.instantRoundEnd);
}

static void DrawTeleportTab() {
    ImGui::TextDisabled("TELEPORT");
    ImGui::Separator();
    ImGui::TextDisabled("Position (fill PLAYER_POS offset to show live coords)");
    ImGui::Spacing();
    if (ImGui::Button("Save Position", {-1, 0}))
        g_state.hasSavedPos = true;   // actual coord read added once PLAYER_POS_OFFSET confirmed
    if (!g_state.hasSavedPos) ImGui::BeginDisabled();
    if (ImGui::Button("Load Position", {-1, 0}))
        g_state.pendingLoadPos = true;
    if (!g_state.hasSavedPos) ImGui::EndDisabled();
    ImGui::Spacing();
    if (ImGui::Button("Teleport to Spawn", {-1, 0}))
        g_state.pendingTeleportSpawn = true;
}
static void DrawVisualsTab() {
    ImGui::TextDisabled("VISUALS");
    ImGui::Separator();

    ImGui::Checkbox("ESP", &g_state.espEnabled);
    if (g_state.espEnabled) {
        ImGui::SameLine();
        ImGui::ColorEdit4("##espcol", (float*)&g_state.espColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }
    ImGui::Checkbox("Custom Crosshair", &g_state.customCrosshair);
    ImGui::SliderFloat("FOV", &g_state.fovOverride, 40.f, 120.f, "%.0f");
    ImGui::Checkbox("Remove Fog", &g_state.removeFog);
}

static void DrawAimbotTab() {
    ImGui::TextDisabled("AIMBOT");
    ImGui::Separator();

    ImGui::Checkbox("Enable Aimbot", &g_state.aimbotEnabled);
    if (!g_state.aimbotEnabled) ImGui::BeginDisabled();

    ImGui::Checkbox("Silent Aim",      &g_state.silentAim);
    ImGui::Checkbox("Aim Prediction",  &g_state.aimPrediction);
    ImGui::Checkbox("Visible Only",    &g_state.visibleOnly);
    ImGui::Checkbox("Draw FOV Circle", &g_state.drawFOVCircle);

    const char* bones[] = {"Head", "Neck", "Chest", "Pelvis"};
    ImGui::Combo("Bone Target", &g_state.boneTarget, bones, 4);

    ImGui::SliderFloat("Aim FOV",    &g_state.aimFOV,    10.f, 180.f, "%.0f deg");
    ImGui::SliderFloat("Smoothness", &g_state.smoothness, 1.f,  20.f, "%.1f");

    if (!g_state.aimbotEnabled) ImGui::EndDisabled();
}
