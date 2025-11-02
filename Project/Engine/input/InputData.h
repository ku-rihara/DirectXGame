#pragma once
#include <array>
#include <cstdint>
#include <dinput.h>
#include <XInput.h>

/// <summary>
/// ゲームパッドボタンの列挙型
/// </summary>
enum class GamepadButton : int32_t {
    A,
    B,
    X,
    Y,
    LB,
    RB,
    Back,
    Start,
    LS,
    RS,
    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,
    COUNT
};
/// <summary>
/// キーボードキーの列挙型
/// </summary>
enum class KeyboardKey : int32_t {
    // 文字キー
    A = DIK_A,
    B = DIK_B,
    C = DIK_C,
    D = DIK_D,
    E = DIK_E,
    F = DIK_F,
    G = DIK_G,
    H = DIK_H,
    I = DIK_I,
    J = DIK_J,
    K = DIK_K,
    L = DIK_L,
    M = DIK_M,
    N = DIK_N,
    O = DIK_O,
    P = DIK_P,
    Q = DIK_Q,
    R = DIK_R,
    S = DIK_S,
    T = DIK_T,
    U = DIK_U,
    V = DIK_V,
    W = DIK_W,
    X = DIK_X,
    Y = DIK_Y,
    Z = DIK_Z,

    // 数字キー
    Key0 = DIK_0,
    Key1 = DIK_1,
    Key2 = DIK_2,
    Key3 = DIK_3,
    Key4 = DIK_4,
    Key5 = DIK_5,
    Key6 = DIK_6,
    Key7 = DIK_7,
    Key8 = DIK_8,
    Key9 = DIK_9,

    // 特殊キー
    Space      = DIK_SPACE,
    Enter      = DIK_RETURN,
    Escape     = DIK_ESCAPE,
    Backspace  = DIK_BACK,
    Tab        = DIK_TAB,
    LeftShift  = DIK_LSHIFT,
    RightShift = DIK_RSHIFT,
    LeftCtrl   = DIK_LCONTROL,
    RightCtrl  = DIK_RCONTROL,
    LeftAlt    = DIK_LALT,
    RightAlt   = DIK_RALT,

    // 矢印キー
    Up    = DIK_UP,
    Down  = DIK_DOWN,
    Left  = DIK_LEFT,
    Right = DIK_RIGHT,

    COUNT
};

/// <summary>
/// ゲームパッドボタンのラベル
/// </summary>
constexpr std::array<const char*, static_cast<int>(GamepadButton::COUNT)> GamepadButtonLabels = {
    "A",
    "B",
    "X",
    "Y",
    "LB",
    "RB",
    "Back",
    "Start",
    "LS",
    "RS",
    "D-Pad Up",
    "D-Pad Down",
    "D-Pad Left",
    "D-Pad Right"};

/// <summary>
/// キーボードキーのラベル
/// </summary>
constexpr std::array<const char*, static_cast<int>(KeyboardKey::COUNT)> KeyboardKeyLabels = {
    // 文字キー
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",

    // 数字キー
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",

    // 特殊キー
    "Space", "Enter", "Escape", "Backspace", "Tab",
    "Left Shift", "Right Shift", "Left Ctrl", "Right Ctrl", "Left Alt", "Right Alt",

    // 矢印キー
    "Up Arrow", "Down Arrow", "Left Arrow", "Right Arrow"};

/// <summary>
/// ImGuiでゲームパッドボタンを選択
/// </summary>
/// <param name="label">ラベル</param>
/// <param name="target">選択されたボタン（int32_t参照）</param>
void ImGuiGamepadButtonSelector(const char* label, int32_t& target);

/// <summary>
/// ImGuiでキーボードキーを選択
/// </summary>
/// <param name="label">ラベル</param>
/// <param name="target">選択されたキー（int32_t参照）</param>
void ImGuiKeyboardKeySelector(const char* label, int32_t& target);

/// <summary>
/// ゲームパッドボタンのインデックスを取得
/// </summary>
/// <param name="buttonValue">ボタン値</param>
/// <returns>インデックス</returns>
int32_t GetGamepadButtonIndex(const int32_t& buttonValue);

/// <summary>
/// キーボードキーのインデックスを取得
/// </summary>
/// <param name="keyValue">キー値</param>
/// <returns>インデックス</returns>
int32_t GetKeyboardKeyIndex(const int32_t& keyValue);


/// <summary>
/// GamepadButton → XINPUT_GAMEPAD_* のビット値に変換
/// </summary>
/// <param name="button">GamepadButton列挙値</param>
/// <returns>XINPUT_GAMEPAD_* のビットフラグ値</returns>
int32_t ToXInputButtonFlag(GamepadButton button);

/// <summary>
/// XINPUT_GAMEPAD_* のビット値 → GamepadButton列挙値に変換
/// </summary>
/// <param name="xinputFlag">XINPUTのボタンビット</param>
/// <returns>対応するGamepadButton</returns>
GamepadButton FromXInputButtonFlag(const int32_t& xinputFlag);