#pragma once
#include <dinput.h>
#include <wrl.h>
#include <variant>

#include"ViewProjection.h"
#include"Vector2.h"

struct MouseMove {
	LONG lX;
	LONG lY;
	LONG lZ;
};

class Mouse {
private:
	//マウス
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 mousePre_;
	Vector2 mousePosition_;
	HWND hWnd_;
public:
	

	//初期化、更新
	void Init(Microsoft::WRL::ComPtr<IDirectInput8>directInput,HWND hWnd);
	void Update();

	/// <summary>
		/// マウスの押下をチェック
		/// </summary>
		/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
		/// <returns>押されているか</returns>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウスのトリガーをチェック。押した瞬間だけtrueになる
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張マウスボタン)</param>
	/// <returns>トリガーか</returns>
	bool IsTriggerMouse(int32_t buttonNumber) const;

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	int32_t GetWheel() const;

	/// <summary>
	/// マウスの位置を取得する（ウィンドウ座標系）
	/// </summary>
	/// <returns>マウスの位置</returns>
	Vector2 GetMousePos() ;

	/// <summary>
	/// 3Dのマウス座標
	/// </summary>
	/// <param name="viewprojection"></param>
	/// <param name="depthFactor"></param>
	/// <returns></returns>
	Vector3 GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSpacing=1.0f) const;

};