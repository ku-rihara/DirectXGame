#pragma once

#include <variant>
//std
#include<wrl.h>
#include<vector>
#include <array>
#include<memory>

#include"Vector2.h"

#define DIRECTNPUT_VERSION 0x0800//バージョン指定
#include <XInput.h>
#include<dinput.h>
//input
#include"Mouse.h"

class Input {

private:
	enum class PadType {
		DirectInput,
		XInput,
	};
	using State = std::variant<DIJOYSTATE2, XINPUT_STATE>;

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
	//マウス
	static std::unique_ptr<Mouse>mouse_;


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

	///// <summary>
	///// 全マウス情報取得
	///// </summary>
	///// <returns>マウス情報</returns>
	//const DIMOUSESTATE2& GetAllMouse() const;

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>押されているか</returns>
	static	bool IsPressMouse(int32_t mouseNumber);

	/// <summary>
	/// マウスのトリガーをチェック。押した瞬間だけtrueになる
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>トリガーか</returns>
	static	bool IsTriggerMouse(int32_t buttonNumber);

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	static	MouseMove GetMouseMove();

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	static	int32_t GetWheel();

	/// <summary>
	/// マウスの位置を取得する（ウィンドウ座標系）
	/// </summary>
	/// <returns>マウスの位置</returns>
	static	 Vector2 GetMousePos();

	/// <summary>
		/// 3Dのマウス座標
		/// </summary>
		/// <param name="viewprojection"></param>
		/// <param name="depthFactor"></param>
		/// <returns></returns>
	static Vector3 GetMousePos3D(const ViewProjection& viewprojection, float depthFactor);


	const BYTE* GetKeyState() const { return key_.data(); }
	const BYTE* GetPreviousKeyState() const { return keyPre_.data(); }
};

