// Input.cpp
#include "Input.h"
#include <assert.h>
#include <string>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

// 静的メンバ変数の定義
std::unique_ptr<Mouse> Input::mouse_                   = nullptr;
std::vector<std::unique_ptr<Gamepad>> Input::gamepads_ = {};

Input* Input::GetInstance() {
    static Input instance;
    return &instance;
}

void Input::Init(HINSTANCE hInstance, HWND hWnd) {
    // DirectInputの初期化
    HRESULT result = DirectInput8Create(
        hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
        (void**)&directInput_, nullptr);
    assert(SUCCEEDED(result));

    // キーボードデバイスの生成
    result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
    assert(SUCCEEDED(result));

    // 入力データ形式のセット
    result = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
    assert(SUCCEEDED(result));

    // 排他制御レベルのセット
    result = keyboard_->SetCooperativeLevel(
        hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(result));

    // マウス初期化
    mouse_ = std::make_unique<Mouse>();
    mouse_->Init(directInput_, hWnd);

    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        auto gamepad = std::make_unique<Gamepad>();
        if (gamepad->Init(directInput_, i)) {
            gamepads_.push_back(std::move(gamepad));
            OutputDebugStringA(("Gamepad " + std::to_string(i) + " initialized successfully\n").c_str()); // デバッグ出力
        } else {
            OutputDebugStringA(("Gamepad " + std::to_string(i) + " initialization failed\n").c_str()); // デバッグ出力
        }
    }

    OutputDebugStringA(("Number of gamepads initialized: " + std::to_string(gamepads_.size()) + "\n").c_str()); // デバッグ出力
}

void Input::Update() {
    keyPre_ = key_;
    // キーボードの情報の取得開始
    keyboard_->Acquire();
    // 全キーの入力状態を取得する
    keyboard_->GetDeviceState(sizeof(key_), key_.data());

    mouse_->Update(); // マウス更新

    for (auto& gamepad : gamepads_) {
        gamepad->Update();
    }
}

// キーボード*************************************************************
bool Input::PushKey(KeyboardKey key) const {
    return (key_[static_cast<BYTE>(ToDIKCode(key))] & 0x80);
}

bool Input::TriggerKey(KeyboardKey key) const {
    return (key_[static_cast<BYTE>(ToDIKCode(key))] & 0x80) && !(keyPre_[static_cast<BYTE>(ToDIKCode(key))] & 0x80);
}

bool Input::ReleaseKey(KeyboardKey key) const {
    return !(key_[static_cast<BYTE>(ToDIKCode(key))] & 0x80);
}

bool Input::ReleaseMomentKey(KeyboardKey key) const {
    return !(key_[static_cast<BYTE>(ToDIKCode(key))] & 0x80) && (keyPre_[static_cast<BYTE>(ToDIKCode(key))] & 0x80);
}

const BYTE* Input::GetKeyState() const {
    return key_.data();
}

const BYTE* Input::GetPreviousKeyState() const {
    return keyPre_.data();
}

// マウス****************************************************************

bool Input::IsPressMouse(int32_t buttonNumber) {
    if (!mouse_)
        return false;
    return mouse_->IsPressMouse(buttonNumber);
}

bool Input::IsTriggerMouse(int32_t buttonNumber) {
    if (!mouse_)
        return false;
    return mouse_->IsTriggerMouse(buttonNumber);
}

MouseMove Input::GetMouseMove() {
    if (!mouse_)
        return MouseMove{};
    return mouse_->GetMouseMove();
}

Vector3 Input::GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSpacing) {
    if (!mouse_)
        return Vector3{};
    return mouse_->GetMousePos3D(viewprojection, depthFactor, blockSpacing);
}

int32_t Input::GetWheel() {
    if (!mouse_)
        return 0;
    return mouse_->GetWheel();
}

Vector2 Input::GetMousePos() {
    if (!mouse_)
        return Vector2{};
    return mouse_->GetMousePos();
}

//-------------------------------------------------------------------------------------------------------
// ゲームパッド
//-------------------------------------------------------------------------------------------------------

bool Input::IsPressPad(int32_t padNumber, GamepadButton button) {
    if (padNumber < 0 || padNumber >= static_cast<int32_t>(gamepads_.size())) {
        return false;
    }

    return gamepads_[padNumber]->IsPressButton((ToXInputButtonFlag(button)));
}

bool Input::IsTriggerPad(int32_t padNumber, GamepadButton button) {
    if (padNumber < 0 || padNumber >= static_cast<int32_t>(gamepads_.size())) {
        return false;
    }

    return gamepads_[padNumber]->IsTriggerButton((ToXInputButtonFlag(button)));
}

Vector2 Input::GetPadStick(int32_t padNumber, int32_t stickNumber) {
    if (padNumber < 0 || padNumber >= static_cast<int32_t>(gamepads_.size())) {
        return Vector2();
    }

    return gamepads_[padNumber]->GetStick(stickNumber);
}

void Input::SetVibration(int32_t padNumber, float leftVelocity, float rightVelocity) {

    if (padNumber < 0 || padNumber >= static_cast<int32_t>(gamepads_.size())) {
        return;
    }
    gamepads_[padNumber]->SetVibration(leftVelocity, rightVelocity);
}

template <typename T>
bool Input::GetJoystickState(int32_t stickNo, T& out) {
    if (stickNo < 0 || stickNo >= static_cast<int32_t>(gamepads_.size())) {
        return false;
    }

    return gamepads_[stickNo]->GetState(out);
}

template <typename T>
bool Input::GetJoystickStatePrevious(int32_t stickNo, T& out) {
    if (stickNo < 0 || stickNo >= static_cast<int32_t>(gamepads_.size())) {
        return false;
    }

    return gamepads_[stickNo]->GetStatePrevious(out);
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) {

    if (stickNo < 0 || stickNo >= static_cast<int32_t>(gamepads_.size())) {
        return;
    }

    gamepads_[stickNo]->SetDeadZone(deadZoneL, deadZoneR);
}

size_t Input::GetNumberOfJoysticks() {
    return gamepads_.size();
}

// テンプレートの明示的インスタンス化
template bool Input::GetJoystickState<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out);
template bool Input::GetJoystickState<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out);
template bool Input::GetJoystickStatePrevious<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out);
template bool Input::GetJoystickStatePrevious<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out);