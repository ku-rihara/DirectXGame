// Input.h (変更なし)
#pragma once

#include <variant>
//std
#include<wrl.h>
#include<vector>
#include <array>
#include<memory>

#include"Vector2.h"

#define DIRECTNPUT_VERSION 0x0800//バージョン指定
#include<dinput.h>
//input
#include"Mouse.h"
#include "Gamepad.h"  // Gamepad.h をインクルード

class Input {
private:
	Microsoft::WRL::ComPtr<IDirectInput8>directInput_ = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	std::array<BYTE, 256> key_;
	std::array<BYTE, 256> keyPre_;
	//マウス
	static std::unique_ptr<Mouse>mouse_;
	static std::vector<std::unique_ptr<Gamepad>> gamepads_; // Gamepadクラスのベクター

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

	///-------------------------------------------------------------------------------------------------------
	/// マウス
	///-------------------------------------------------------------------------------------------------------

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
	static Vector3 GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSpacing = 1.0f);


	const BYTE* GetKeyState() const { return key_.data(); }
	const BYTE* GetPreviousKeyState() const { return keyPre_.data(); }

	//-------------------------------------------------------------------------------------------------------
	// ゲームパッド
	//-------------------------------------------------------------------------------------------------------

	/// <summary>
	/// ゲームパッドの入力をチェック
	/// </summary>
	/// <param name="padNumber">パッド番号</param>
	/// <param name="buttonNumber">ボタン番号</param>
	/// <returns>押されているか</returns>
	static bool IsPressPad(int32_t padNumber, int32_t buttonNumber);

	/// <summary>
	/// ゲームパッドのトリガーをチェック
	/// </summary>
	/// <param name="padNumber">パッド番号</param>
	/// <param name="buttonNumber">ボタン番号</param>
	/// <returns>トリガーか</returns>
	static bool IsTriggerPad(int32_t padNumber, int32_t buttonNumber);

	/// <summary>
	/// ゲームパッドのスティックの入力を取得
	/// </summary>
	/// <param name="padNumber">パッド番号</param>
	/// <param name="stickNumber">スティック番号(0:左,1:右)</param>
	/// <returns>スティックの入力</returns>
	static Vector2 GetPadStick(int32_t padNumber, int32_t stickNumber);

	/// <summary>
	/// ゲームパッドの振動
	/// </summary>
	/// <param name="padNumber">パッド番号</param>
	///  /// <param name="leftVelocity">左モーター</param>
	///   /// <param name="rightVelocity">右モーター</param>
	static void SetVibration(int32_t padNumber, float leftVelocity, float rightVelocity);

	///// <summary>
	///// 現在のジョイスティック状態を取得する
	///// </summary>
	///// <param name="stickNo">ジョイスティック番号</param>
	///// <param name="out">現在のジョイスティック状態</param>
	///// <returns>正しく取得できたか</returns>
	//template<typename T> bool GetJoystickState(int32_t stickNo, T& out) const;

	///// <summary>
	///// 前回のジョイスティック状態を取得する
	///// </summary>
	///// <param name="stickNo">ジョイスティック番号</param>
	///// <param name="out">前回のジョイスティック状態</param>
	///// <returns>正しく取得できたか</returns>
	//template<typename T> bool GetJoystickStatePrevious(int32_t stickNo, T& out) const;

	///// <summary>
	///// デッドゾーンを設定する
	///// </summary>
	///// <param name="stickNo">ジョイスティック番号</param>
	///// <param name="deadZoneL">デッドゾーン左スティック 0~32768</param>
	///// <param name="deadZoneR">デッドゾーン右スティック 0~32768</param>
	///// <returns>正しく取得できたか</returns>
	//void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

	///// <summary>
	///// 接続されているジョイスティック数を取得する
	///// </summary>
	///// <returns>接続されているジョイスティック数</returns>
	//size_t GetNumberOfJoysticks()const;
};
