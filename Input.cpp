#include "Input.h"
#include<assert.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

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
}

void Input::Update() {
	keyPre_ = key_;
	//キーボードの情報の取得開始
	keyboard_->Acquire();
	//全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_.data());
}


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