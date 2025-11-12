#pragma once
#include <array>
#include <cstdint>
#include <dinput.h>
#include <XInput.h>

/// <summary>
/// ゲームパッドボタンの列挙型
/// </summary>
enum class GamepadButton {
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
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // 数字キー
    Key0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,

    // 特殊キー
    Space,
    Enter,
    Escape,
    Backspace,
    Tab,
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,

    // 矢印キー
    Up,
    Down,
    Left,
    Right,

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

/// <summary>
/// KeyboardKey列挙型からDirectInputのキーコードに変換
/// </summary>
/// <param name="key">KeyboardKey列挙型のキー</param>
/// <returns>対応するDirectInputのキーコード</returns>
int32_t ToDIKCode(KeyboardKey key);

/// <summary>
/// DirectInputのキーコードからKeyboardKey列挙型に変換
/// </summary>
/// <param name="dikCode">DirectInputのキーコード</param>
/// <returns>対応するKeyboardKey列挙型のキー</returns>
KeyboardKey FromDIKCode(const int32_t& dikCode);