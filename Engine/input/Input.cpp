#include "Input.h"
#include<assert.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

std::unique_ptr<Mouse>Input::mouse_ = nullptr;

template bool Input::GetJoystickState<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out) const;
template bool Input::GetJoystickState<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out) const;
template bool Input::GetJoystickStatePrevious<DIJOYSTATE2>(int32_t stickNo, DIJOYSTATE2& out) const;
template bool Input::GetJoystickStatePrevious<XINPUT_STATE>(int32_t stickNo, XINPUT_STATE& out) const;

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Init(HINSTANCE hInstance, HWND hWnd) {
	//DirectInputの初期化
	HRESULT result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(
		hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	//マウス初期化
	mouse_ = std::make_unique<Mouse>();
	mouse_->Init(directInput_,hWnd);

	//XInputデバイスの追加
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			Joystick joystick = {};
			joystick.type_ = PadType::XInput;
			joystick.state_ = state;
			joystick.statePre_ = state;
			joystick.deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			joystick.deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			joysticks_.push_back(joystick);
		}
	}
}

void Input::Update() {
	keyPre_ = key_;
	//キーボードの情報の取得開始
	keyboard_->Acquire();
	//全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_.data());

	mouse_->Update();//マウス更新


	// マウスの位置を更新
	/*mousePosition_.x += static_cast<float>(mouse_.lX);
	mousePosition_.y += static_cast<float>(mouse_.lY);*/

	for (auto& joystick : joysticks_) {
		if (joystick.type_ == PadType::XInput) {
			DWORD dwResult;
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			dwResult = XInputGetState(0, &state);
			if (dwResult == ERROR_SUCCESS) {
				joystick.statePre_ = joystick.state_;
				joystick.state_ = state;
			}
		}
	}
}

//キーボード*************************************************************
bool Input::PushKey(BYTE keyNumber)const {
	return (key_[keyNumber] & 0x80);
}


bool Input::TrrigerKey(BYTE keyNumber)const {
	return (key_[keyNumber] & 0x80) && !(keyPre_[keyNumber] & 0x80);
}


bool Input::ReleaseKey(BYTE keyNumber)const {
	return !(key_[keyNumber] & 0x80);
}


bool Input::ReleaseMomentKey(BYTE keyNumber)const {
	return !(key_[keyNumber] & 0x80) && (keyPre_[keyNumber] & 0x80);
}

//ゲームパッド*******************************************************************

template<typename T>bool Input::GetJoystickState(int32_t stickNo, T& out)const {
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return false;
	}
	const Joystick& joystick = joysticks_[stickNo];
	if (joystick.type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			if (std::holds_alternative<T>(joystick.state_)) {
				out = std::get<T>(joystick.state_);
				return true;
			}
		}
	}
	else if (joystick.type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			if (std::holds_alternative<T>(joystick.state_)) {
				out = std::get<T>(joystick.state_);
				return true;
			}
		}
	}
	return false;
}


template<typename T>bool Input::GetJoystickStatePrevious(int32_t stickNo, T& out) const {
	if (stickNo < 0 || stickNo >= joysticks_.size()) {
		return false;
	}
	const Joystick& joystick = joysticks_[stickNo];
	if (joystick.type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			if (std::holds_alternative<T>(joystick.statePre_)) {
				out = std::get<T>(joystick.statePre_);
				return true;
			}
		}
	}
	else if (joystick.type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			if (std::holds_alternative<T>(joystick.statePre_)) {
				out = std::get<T>(joystick.statePre_);
				return true;
			}
		}
	}
	return false;
}

void Input::SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR) {

	if (stickNo < 0 || stickNo >= static_cast<int32_t>(joysticks_.size())) {
		return;
	}

	joysticks_[stickNo].deadZoneL_ = deadZoneL;
	joysticks_[stickNo].deadZoneR_ = deadZoneR;
}

size_t Input::GetNumberOfJoysticks()const {
	return joysticks_.size();
}

//マウス****************************************************************

bool Input::IsPressMouse(int32_t buttonNumber) {
	return mouse_->IsPressMouse(buttonNumber);
}

bool Input::IsTriggerMouse(int32_t buttonNumber) {
	return mouse_->IsTriggerMouse(buttonNumber);
}

MouseMove Input::GetMouseMove() {
	return mouse_->GetMouseMove();
}

Vector3 Input::GetMousePos3D(const ViewProjection& viewprojection, float depthFactor) {
	return mouse_->GetMousePos3D(viewprojection, depthFactor);
}


int32_t Input::GetWheel()  {
	return mouse_->GetWheel();
}

 Vector2 Input::GetMousePos()  {
	 return mouse_->GetMousePos();
}