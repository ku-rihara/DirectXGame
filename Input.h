#pragma once
#define DIRECTNPUT_VERSION 0x0800//バージョン指定
#include<dinput.h>
#include <XInput.h>
#include <variant>
#include<wrl.h>
#include<vector>
#include <array>

class Input {
private:
	enum class PadType {
		DirectInput,
		XInput,
	};
	using State= std::variant<DIJOYSTATE2, XINPUT_STATE>;

	struct Joystick {
		Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
		int32_t deadZoneL_;
		int32_t deadZoneR_;
		PadType type_;
		State state_;
		State statePre_;
	};
private:
	Microsoft::WRL::ComPtr<IDirectInput8>directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
	std::vector<Joystick>joysticks_;
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

	/// <summary>
	/// 現在のジョイスティック状態を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">現在のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	template<typename T> bool GetJoystickState(int32_t stickNo, T& out) const;

	/// <summary>
	/// 前回のジョイスティック状態を取得する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="out">前回のジョイスティック状態</param>
	/// <returns>正しく取得できたか</returns>
	template<typename T> bool GetJoystickStatePrevious(int32_t stickNo, T& out) const;

	/// <summary>
	/// デッドゾーンを設定する
	/// </summary>
	/// <param name="stickNo">ジョイスティック番号</param>
	/// <param name="deadZoneL">デッドゾーン左スティック 0~32768</param>
	/// <param name="deadZoneR">デッドゾーン右スティック 0~32768</param>
	/// <returns>正しく取得できたか</returns>
	void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

	/// <summary>
	/// 接続されているジョイスティック数を取得する
	/// </summary>
	/// <returns>接続されているジョイスティック数</returns>
	size_t GetNumberOfJoysticks()const;


	const BYTE* GetKeyState() const { return key_.data(); }
	const BYTE* GetPreviousKeyState() const { return keyPre_.data(); }
};

