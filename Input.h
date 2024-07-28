#pragma once
#define DIRECTNPUT_VERSION 0x0800//バージョン指定
#include<dinput.h>
#include<wrl.h>
#include <array>

class Input{
private:
	Microsoft::WRL::ComPtr<IDirectInput8>directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
public:
	// シングルトンインスタンスの取得
	static Input* GetInstance();
	void Init(HINSTANCE hInstance,HWND hwnd);
	void Update();
};

