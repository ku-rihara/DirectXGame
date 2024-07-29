#pragma once
#define DIRECTNPUT_VERSION 0x0800//バージョン指定
#include<dinput.h>
#include<wrl.h>
#include <array>

class Input {
private:
	Microsoft::WRL::ComPtr<IDirectInput8>directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
public:
	// シングルトンインスタンスの取得
	static Input* GetInstance();
	void Init(HINSTANCE hInstance, HWND hwnd);
	void Update();

	/// <summary>
	/// 押し込んでいるか
	/// </summary>
	/// <param name="キー番号"></param>
	/// <returns></returns>
	bool PushKey(BYTE keyNumber)const;

	/// <summary>
	/// トリガーしているか
	/// </summary>
	/// <param name="キー番号"></param>
	/// <returns></returns>
	bool TrrigerKey(BYTE keyNumber)const;

	/// <summary>
	///　離しているか
	/// </summary>
	/// <param name="キー番号"></param>
	/// <returns></returns>
	bool ReleaseKey(BYTE keyNumber)const;

	/// <summary>
    ///　離した瞬間か
    /// </summary>
    /// <param name="キー番号"></param>
    /// <returns></returns>
	bool ReleaseMomentKey(BYTE keyNumber)const;


	const BYTE* GetKeyState() const { return key_.data(); }
	const BYTE* GetPreviousKeyState() const { return keyPre_.data(); }
};

