#include "imgui_impl_moss.h"

#include "imgui.h"
#include <Moss/Moss_Platform.h>

#include <chrono>

namespace {
struct BackendData {
    Moss_Window* window = nullptr;
    std::chrono::steady_clock::time_point lastFrame{};
};

BackendData* g_backend = nullptr;

struct KeyMap { ImGuiKey imgui; Keyboard moss; };

constexpr KeyMap kKeys[] = {
    {ImGuiKey_Tab, Keyboard::KEY_TAB}, {ImGuiKey_LeftArrow, Keyboard::KEY_LEFT},
    {ImGuiKey_RightArrow, Keyboard::KEY_RIGHT}, {ImGuiKey_UpArrow, Keyboard::KEY_UP},
    {ImGuiKey_DownArrow, Keyboard::KEY_DOWN}, {ImGuiKey_PageUp, Keyboard::KEY_PAGE_UP},
    {ImGuiKey_PageDown, Keyboard::KEY_PAGE_DOWN}, {ImGuiKey_Home, Keyboard::KEY_HOME},
    {ImGuiKey_End, Keyboard::KEY_END}, {ImGuiKey_Insert, Keyboard::KEY_INSERT},
    {ImGuiKey_Delete, Keyboard::KEY_DELETE}, {ImGuiKey_Backspace, Keyboard::KEY_BACKSPACE},
    {ImGuiKey_Space, Keyboard::KEY_SPACE}, {ImGuiKey_Enter, Keyboard::KEY_ENTER},
    {ImGuiKey_Escape, Keyboard::KEY_ESCAPE}, {ImGuiKey_LeftCtrl, Keyboard::KEY_LEFT_CONTROL},
    {ImGuiKey_LeftShift, Keyboard::KEY_LEFT_SHIFT}, {ImGuiKey_LeftAlt, Keyboard::KEY_LEFT_ALT},
    {ImGuiKey_LeftSuper, Keyboard::KEY_LEFT_SUPER}, {ImGuiKey_RightCtrl, Keyboard::KEY_RIGHT_CONTROL},
    {ImGuiKey_RightShift, Keyboard::KEY_RIGHT_SHIFT}, {ImGuiKey_RightAlt, Keyboard::KEY_RIGHT_ALT},
    {ImGuiKey_RightSuper, Keyboard::KEY_RIGHT_SUPER}, {ImGuiKey_Menu, Keyboard::KEY_MENU},
    {ImGuiKey_0, Keyboard::KEY_0}, {ImGuiKey_1, Keyboard::KEY_1},
    {ImGuiKey_2, Keyboard::KEY_2}, {ImGuiKey_3, Keyboard::KEY_3},
    {ImGuiKey_4, Keyboard::KEY_4}, {ImGuiKey_5, Keyboard::KEY_5},
    {ImGuiKey_6, Keyboard::KEY_6}, {ImGuiKey_7, Keyboard::KEY_7},
    {ImGuiKey_8, Keyboard::KEY_8}, {ImGuiKey_9, Keyboard::KEY_9},
    {ImGuiKey_A, Keyboard::KEY_A}, {ImGuiKey_B, Keyboard::KEY_B},
    {ImGuiKey_C, Keyboard::KEY_C}, {ImGuiKey_D, Keyboard::KEY_D},
    {ImGuiKey_E, Keyboard::KEY_E}, {ImGuiKey_F, Keyboard::KEY_F},
    {ImGuiKey_G, Keyboard::KEY_G}, {ImGuiKey_H, Keyboard::KEY_H},
    {ImGuiKey_I, Keyboard::KEY_I}, {ImGuiKey_J, Keyboard::KEY_J},
    {ImGuiKey_K, Keyboard::KEY_K}, {ImGuiKey_L, Keyboard::KEY_L},
    {ImGuiKey_M, Keyboard::KEY_M}, {ImGuiKey_N, Keyboard::KEY_N},
    {ImGuiKey_O, Keyboard::KEY_O}, {ImGuiKey_P, Keyboard::KEY_P},
    {ImGuiKey_Q, Keyboard::KEY_Q}, {ImGuiKey_R, Keyboard::KEY_R},
    {ImGuiKey_S, Keyboard::KEY_S}, {ImGuiKey_T, Keyboard::KEY_T},
    {ImGuiKey_U, Keyboard::KEY_U}, {ImGuiKey_V, Keyboard::KEY_V},
    {ImGuiKey_W, Keyboard::KEY_W}, {ImGuiKey_X, Keyboard::KEY_X},
    {ImGuiKey_Y, Keyboard::KEY_Y}, {ImGuiKey_Z, Keyboard::KEY_Z},
    {ImGuiKey_F1, Keyboard::KEY_F1}, {ImGuiKey_F2, Keyboard::KEY_F2},
    {ImGuiKey_F3, Keyboard::KEY_F3}, {ImGuiKey_F4, Keyboard::KEY_F4},
    {ImGuiKey_F5, Keyboard::KEY_F5}, {ImGuiKey_F6, Keyboard::KEY_F6},
    {ImGuiKey_F7, Keyboard::KEY_F7}, {ImGuiKey_F8, Keyboard::KEY_F8},
    {ImGuiKey_F9, Keyboard::KEY_F9}, {ImGuiKey_F10, Keyboard::KEY_F10},
    {ImGuiKey_F11, Keyboard::KEY_F11}, {ImGuiKey_F12, Keyboard::KEY_F12},
    {ImGuiKey_Apostrophe, Keyboard::KEY_APOSTROPHE}, {ImGuiKey_Comma, Keyboard::KEY_COMMA},
    {ImGuiKey_Minus, Keyboard::KEY_MINUS}, {ImGuiKey_Period, Keyboard::KEY_PERIOD},
    {ImGuiKey_Slash, Keyboard::KEY_SLASH}, {ImGuiKey_Semicolon, Keyboard::KEY_SEMICOLON},
    {ImGuiKey_Equal, Keyboard::KEY_EQUAL}, {ImGuiKey_LeftBracket, Keyboard::KEY_LEFT_BRACKET},
    {ImGuiKey_Backslash, Keyboard::KEY_BACKSLASH}, {ImGuiKey_RightBracket, Keyboard::KEY_RIGHT_BRACKET},
    {ImGuiKey_GraveAccent, Keyboard::KEY_GRAVE_ACCENT}, {ImGuiKey_CapsLock, Keyboard::KEY_CAPS_LOCK},
    {ImGuiKey_ScrollLock, Keyboard::KEY_SCROLL_LOCK}, {ImGuiKey_NumLock, Keyboard::KEY_NUM_LOCK},
    {ImGuiKey_PrintScreen, Keyboard::KEY_PRINT_SCREEN}, {ImGuiKey_Pause, Keyboard::KEY_PAUSE},
    {ImGuiKey_Keypad0, Keyboard::KEY_KP_0}, {ImGuiKey_Keypad1, Keyboard::KEY_KP_1},
    {ImGuiKey_Keypad2, Keyboard::KEY_KP_2}, {ImGuiKey_Keypad3, Keyboard::KEY_KP_3},
    {ImGuiKey_Keypad4, Keyboard::KEY_KP_4}, {ImGuiKey_Keypad5, Keyboard::KEY_KP_5},
    {ImGuiKey_Keypad6, Keyboard::KEY_KP_6}, {ImGuiKey_Keypad7, Keyboard::KEY_KP_7},
    {ImGuiKey_Keypad8, Keyboard::KEY_KP_8}, {ImGuiKey_Keypad9, Keyboard::KEY_KP_9},
    {ImGuiKey_KeypadDecimal, Keyboard::KEY_KP_DECIMAL}, {ImGuiKey_KeypadDivide, Keyboard::KEY_KP_DIVIDE},
    {ImGuiKey_KeypadMultiply, Keyboard::KEY_KP_MULTIPLY}, {ImGuiKey_KeypadSubtract, Keyboard::KEY_KP_SUBTRACT},
    {ImGuiKey_KeypadAdd, Keyboard::KEY_KP_ADD}, {ImGuiKey_KeypadEnter, Keyboard::KEY_KP_ENTER},
    {ImGuiKey_KeypadEqual, Keyboard::KEY_KP_EQUAL}
};

const char* GetClipboardText(ImGuiContext*) {
    const char* text = Moss_GetClipboardText();
    return text ? text : "";
}

void SetClipboardText(ImGuiContext*, const char* text) { Moss_SetClipboardText(text); }
}

bool ImGui_ImplMoss_InitForOpenGL(Moss_Window* window)
{
    if (!window || g_backend || ImGui::GetCurrentContext() == nullptr) return false;
    g_backend = new BackendData{};
    g_backend->window = window;
    g_backend->lastFrame = std::chrono::steady_clock::now();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
    io.BackendPlatformName = "imgui_impl_moss";
    platformIO.Platform_ClipboardUserData = window;
    platformIO.Platform_GetClipboardTextFn = GetClipboardText;
    platformIO.Platform_SetClipboardTextFn = SetClipboardText;
    return true;
}

void ImGui_ImplMoss_Shutdown()
{
    if (!g_backend) return;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
    io.BackendPlatformName = nullptr;
    platformIO.Platform_ClipboardUserData = nullptr;
    platformIO.Platform_GetClipboardTextFn = nullptr;
    platformIO.Platform_SetClipboardTextFn = nullptr;
    delete g_backend;
    g_backend = nullptr;
}

void ImGui_ImplMoss_NewFrame()
{
    IM_ASSERT(g_backend != nullptr && "ImGui Moss backend is not initialized");
    ImGuiIO& io = ImGui::GetIO();
    const auto now = std::chrono::steady_clock::now();
    io.DeltaTime = std::chrono::duration<float>(now - g_backend->lastFrame).count();
    if (io.DeltaTime <= 0.0f) io.DeltaTime = 1.0f / 60.0f;
    g_backend->lastFrame = now;

    io.DisplaySize = ImVec2(static_cast<float>(Moss_GetWindowWidth()), static_cast<float>(Moss_GetWindowHeight()));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    int mouseX = 0, mouseY = 0;
    Moss_GetMousePosition(&mouseX, &mouseY);
    io.AddMousePosEvent(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.AddMouseButtonEvent(0, Moss_IsMousePressed(Moss_Mouse::LEFT));
    io.AddMouseButtonEvent(1, Moss_IsMousePressed(Moss_Mouse::RIGHT));
    io.AddMouseButtonEvent(2, Moss_IsMousePressed(Moss_Mouse::MIDDLE));
    io.AddMouseButtonEvent(3, Moss_IsMousePressed(Moss_Mouse::BUTTON_4));
    io.AddMouseButtonEvent(4, Moss_IsMousePressed(Moss_Mouse::BUTTON_5));
    float wheelX = 0.0f, wheelY = 0.0f;
    Moss_GetMouseWheelDelta(&wheelX, &wheelY);
    if (wheelX != 0.0f || wheelY != 0.0f) io.AddMouseWheelEvent(wheelX, wheelY);

    for (const KeyMap& key : kKeys) io.AddKeyEvent(key.imgui, Moss_IsKeyPressed(key.moss));
    io.AddKeyEvent(ImGuiMod_Ctrl, Moss_IsKeyPressed(Keyboard::KEY_LEFT_CONTROL) || Moss_IsKeyPressed(Keyboard::KEY_RIGHT_CONTROL));
    io.AddKeyEvent(ImGuiMod_Shift, Moss_IsKeyPressed(Keyboard::KEY_LEFT_SHIFT) || Moss_IsKeyPressed(Keyboard::KEY_RIGHT_SHIFT));
    io.AddKeyEvent(ImGuiMod_Alt, Moss_IsKeyPressed(Keyboard::KEY_LEFT_ALT) || Moss_IsKeyPressed(Keyboard::KEY_RIGHT_ALT));
    io.AddKeyEvent(ImGuiMod_Super, Moss_IsKeyPressed(Keyboard::KEY_LEFT_SUPER) || Moss_IsKeyPressed(Keyboard::KEY_RIGHT_SUPER));
    io.AddFocusEvent(Moss_IsWindowFocused(g_backend->window));

    uint32_t text[64] = {};
    const uint32_t count = Moss_GetTextInput(text, 64);
    for (uint32_t index = 0; index < count; ++index) io.AddInputCharacter(text[index]);
}
