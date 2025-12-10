#pragma once

#include <array>
#include <memory>
#include <variant>
#include <vector>
#include <wrl.h>

#include "Vector2.h"

#define DIRECTNPUT_VERSION 0x0800
#include "Gamepad.h"
#include "InputData.h"
#include "Mouse.h"
#include <dinput.h>

/// <summary>
/// 入力管理統合クラス
/// </summary>
namespace KetaEngine {

class Input {
private:
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_    = nullptr;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
    std::array<BYTE, 256> key_;
    std::array<BYTE, 256> keyPre_;
    static std::unique_ptr<Mouse> mouse_;
    static std::vector<std::unique_ptr<Gamepad>> gamepads_;

public:
    static Input* GetInstance();
    Input()  = default;
    ~Input() = default;

    /// <summary>
    /// 入力システムの初期化
    /// </summary>
    /// <param name="hInstance">インスタンスハンドル</param>
    /// <param name="hwnd">ウィンドウハンドル</param>
    void Init(HINSTANCE hInstance, HWND hwnd);
    void Update(); //< 入力状態を更新

    //*------------------------------------ キーボード ------------------------------------*//

    bool PushKey(KeyboardKey key) const; //< キーが押されているか
    bool TriggerKey(KeyboardKey key) const; //< キーがトリガーされたか
    bool ReleaseKey(KeyboardKey key) const; //< キーが離されているか
    bool ReleaseMomentKey(KeyboardKey key) const; //< キーが離された瞬間か

    const BYTE* GetKeyState() const;
    const BYTE* GetPreviousKeyState() const;

    //*------------------------------------ マウス ------------------------------------*//

    // マウス
    /// <summary>
    /// マウスの押下をチェック
    /// </summary>
    /// <param name="mouseNumber">マウスボタン番号(0:左,1:右,2:中,3~7:拡張)</param>
    /// <returns>押下状態</returns>
    static bool IsPressMouse(int32_t mouseNumber);

    static bool IsTriggerMouse(int32_t buttonNumber); //< マウスのトリガーをチェック
    static MouseMove GetMouseMove(); //< マウス移動量を取得
    static int32_t GetWheel(); //< ホイールスクロール量を取得
    static Vector2 GetMousePos(); //< マウス位置を取得

    /// <summary>
    /// 3Dワールド座標でのマウス位置を取得
    /// </summary>
    /// <param name="viewprojection">ビュープロジェクション</param>
    /// <param name="depthFactor">深度係数</param>
    /// <param name="blockSpacing">ブロック間隔</param>
    /// <returns>3Dマウス座標</returns>
    static Vector3 GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSpacing = 1.0f);

    //*------------------------------------ ゲームパッド ------------------------------------*//

    // ゲームパッド
    /// <summary>
    /// ゲームパッドの入力をチェック（列挙型使用）
    /// </summary>
    /// <param name="padNumber">パッド番号</param>
    /// <param name="button">ボタン（列挙型）</param>
    /// <returns>押下状態</returns>
    static bool IsPressPad(int32_t padNumber, GamepadButton button);

    /// <summary>
    /// ゲームパッドのトリガーをチェック（列挙型使用）
    /// </summary>
    /// <param name="padNumber">パッド番号</param>
    /// <param name="button">ボタン（列挙型）</param>
    /// <returns>トリガー状態</returns>
    static bool IsTriggerPad(int32_t padNumber, GamepadButton button);

    /// <summary>
    /// ゲームパッドのスティック入力を取得
    /// </summary>
    /// <param name="padNumber">パッド番号</param>
    /// <param name="stickNumber">スティック番号(0:左,1:右)</param>
    /// <returns>スティック入力値</returns>
    static Vector2 GetPadStick(int32_t padNumber, int32_t stickNumber);

    /// <summary>
    /// ゲームパッドの振動
    /// </summary>
    /// <param name="padNumber">パッド番号</param>
    /// <param name="leftVelocity">左モーター強度</param>
    /// <param name="rightVelocity">右モーター強度</param>
    static void SetVibration(int32_t padNumber, float leftVelocity, float rightVelocity);

    /// <summary>
    /// 現在のジョイスティック状態を取得
    /// </summary>
    /// <param name="stickNo">ジョイスティック番号</param>
    /// <param name="out">現在の状態</param>
    /// <returns>取得成功フラグ</returns>
    template <typename T>
    static bool GetJoystickState(int32_t stickNo, T& out);

    /// <summary>
    /// 前回のジョイスティック状態を取得
    /// </summary>
    /// <param name="stickNo">ジョイスティック番号</param>
    /// <param name="out">前回の状態</param>
    /// <returns>取得成功フラグ</returns>
    template <typename T>
    static bool GetJoystickStatePrevious(int32_t stickNo, T& out);

    /// <summary>
    /// デッドゾーンを設定
    /// </summary>
    /// <param name="stickNo">ジョイスティック番号</param>
    /// <param name="deadZoneL">左スティックデッドゾーン(0~32768)</param>
    /// <param name="deadZoneR">右スティックデッドゾーン(0~32768)</param>
    static void SetJoystickDeadZone(int32_t stickNo, int32_t deadZoneL, int32_t deadZoneR);

    static size_t GetNumberOfJoysticks(); //< 接続されているジョイスティック数を取得
};

}; // KetaEngine
