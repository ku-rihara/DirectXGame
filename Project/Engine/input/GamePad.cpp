// Gamepad.cpp
#include "Gamepad.h"
#include <cmath>


bool Gamepad::Init(Microsoft::WRL::ComPtr<IDirectInput8> directInput, DWORD padNo) {
    directInput;
	padNo_ = padNo;
	// XInput デバイスとして初期化を試みる
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (XInputGetState(padNo, &state) == ERROR_SUCCESS) {
		// XInput デバイスが見つかった
		type_ = PadType::XInput;
		deadZoneL_ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		deadZoneR_ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		state_.xi = state;
		statePre_.xi = state;
		return true;
	}

	
	return false;
}

void Gamepad::Update() {
	if (type_ == PadType::XInput) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD result = XInputGetState(padNo_, &state);
		if (result == ERROR_SUCCESS) {
			statePre_.xi = state_.xi;
			state_.xi = state;
		}
	} else if (type_ == PadType::DirectInput) {
	
	}
}

bool Gamepad::IsPressButton(int32_t buttonNumber) const {
	if (type_ == PadType::XInput) {
		return (state_.xi.Gamepad.wButtons &  buttonNumber) != 0;
	} else if (type_ == PadType::DirectInput) {
	
		return false;
	}
	return false;
}

bool Gamepad::IsTriggerButton(int32_t buttonNumber) const {
	if (type_ == PadType::XInput) {
		bool current = (state_.xi.Gamepad.wButtons & buttonNumber) != 0;
		bool previous = (statePre_.xi.Gamepad.wButtons & buttonNumber) != 0;

		return current && !previous;
	} else if (type_ == PadType::DirectInput) {
		
		return false;
	}
	return false;
}

Vector2 Gamepad::GetStick(int32_t stickNumber) const {
	Vector2 stick = { 0,0 };
	if (type_ == PadType::XInput) {
		float x, y;
		if (stickNumber == 0) {
			x = (float)state_.xi.Gamepad.sThumbLX;
			y = (float)state_.xi.Gamepad.sThumbLY;

			// デッドゾーン処理
			float magnitude = sqrtf(x * x + y * y);
			if (magnitude > deadZoneL_) {
				// スティックの値の範囲を[0, 1]に正規化
				if (magnitude > 32767) {
					magnitude = 32767;
				}
				x = x / magnitude * ((magnitude - deadZoneL_) / (32767 - deadZoneL_));
				y = y / magnitude * ((magnitude - deadZoneL_) / (32767 - deadZoneL_));
			} else {
				x = 0.0f;
				y = 0.0f;
			}
		} else {
			x = (float)state_.xi.Gamepad.sThumbRX;
			y = (float)state_.xi.Gamepad.sThumbRY;
			// デッドゾーン処理
			float magnitude = sqrtf(x * x + y * y);
			if (magnitude > deadZoneR_) {
				// スティックの値の範囲を[0, 1]に正規化
				if (magnitude > 32767) {
					magnitude = 32767;
				}
				x = x / magnitude * ((magnitude - deadZoneR_) / (32767 - deadZoneR_));
				y = y / magnitude * ((magnitude - deadZoneR_) / (32767 - deadZoneR_));
			} else {
				x = 0.0f;
				y = 0.0f;
			}
		}
		stick = { x, y };
		return stick;
	} else if (type_ == PadType::DirectInput) {
		// TODO: DirectInput のスティック入力処理を実装
		return stick;
	}
	return stick;
}

template<typename T> bool Gamepad::GetState(T& out) const {
	if (type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			out = state_.xi;
			return true;
		}
	} else if (type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			out = state_.di;
			return true;
		}
	}
	return false;
}

template<typename T> bool Gamepad::GetStatePrevious(T& out) const {
	if (type_ == PadType::XInput) {
		if constexpr (std::is_same<T, XINPUT_STATE>::value) {
			out = statePre_.xi;
			return true;
		}
	} else if (type_ == PadType::DirectInput) {
		if constexpr (std::is_same<T, DIJOYSTATE2>::value) {
			out = statePre_.di;
			return true;
		}
	}
	return false;
}

void Gamepad::SetDeadZone(int32_t deadZoneL, int32_t deadZoneR) {
	deadZoneL_ = deadZoneL;
	deadZoneR_ = deadZoneR;
}

void Gamepad::SetVibration(float leftVelocity, float rightVelocity) {
	if (type_ == PadType::XInput) {
		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = static_cast<WORD>(leftVelocity * 65535.0f);
		vibration.wRightMotorSpeed = static_cast<WORD>(rightVelocity * 65535.0f); 
		XInputSetState(padNo_, &vibration);
	} else if (type_ == PadType::DirectInput) {
		// TODO: DirectInput の振動処理を実装
	}
}




template bool Gamepad::GetState<DIJOYSTATE2>(DIJOYSTATE2& out)const;
template bool Gamepad::GetState<XINPUT_STATE>(XINPUT_STATE& out)const;
template bool Gamepad::GetStatePrevious<DIJOYSTATE2>(DIJOYSTATE2& out)const;
template bool Gamepad::GetStatePrevious<XINPUT_STATE>(XINPUT_STATE& out)const;