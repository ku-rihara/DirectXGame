#pragma once
#include "Vector2.h"
#include <cstdint>
#include <dinput.h>
#include <wrl.h>
#include <XInput.h>

/// <summary>
/// ゲームパッド入力管理クラス(XInput)
/// </summary>
namespace KetaEngine {

class Gamepad {
private:
    enum class PadType {
        DirectInput,
        XInput,
        None,
    };

    union State {
        DIJOYSTATE2 di;
        XINPUT_STATE xi;
    };

    PadType type_      = PadType::None;
    int32_t deadZoneL_ = 0;
    int32_t deadZoneR_ = 0;
    State state_;
    State statePre_;
    DWORD padNo_ = 0;

    Microsoft::WRL::ComPtr<IDirectInputDevice8> device_ = nullptr;

public:
    Gamepad()  = default;
    ~Gamepad() = default;

    /// <summary>
    /// ゲームパッドの初期化
    /// </summary>
    /// <param name="directInput">DirectInputインスタンス</param>
    /// <param name="padNo">パッド番号</param>
    /// <returns>初期化成功フラグ</returns>
    bool Init(Microsoft::WRL::ComPtr<IDirectInput8> directInput, DWORD padNo);

    void Update(); //< ゲームパッド状態を更新

    /// <summary>
    /// ボタンが押されているか
    /// </summary>
    /// <param name="buttonNumber">ボタン番号</param>
    /// <returns>押下状態</returns>
    bool IsPressButton(int32_t buttonNumber) const;

    /// <summary>
    /// ボタンがトリガーされたか
    /// </summary>
    /// <param name="buttonNumber">ボタン番号</param>
    /// <returns>トリガー状態</returns>
    bool IsTriggerButton(int32_t buttonNumber) const;

    /// <summary>
    /// スティックの入力を取得
    /// </summary>
    /// <param name="stickNumber">スティック番号(0:左, 1:右)</param>
    /// <returns>スティック入力値</returns>
    Vector2 GetStick(int32_t stickNumber) const;

    template <typename T>
    bool GetState(T& out) const;
    template <typename T>
    bool GetStatePrevious(T& out) const;

    /// <summary>
    /// デッドゾーン設定
    /// </summary>
    /// <param name="deadZoneL">左スティックデッドゾーン</param>
    /// <param name="deadZoneR">右スティックデッドゾーン</param>
    void SetDeadZone(int32_t deadZoneL, int32_t deadZoneR);

    /// <summary>
    /// 振動設定
    /// </summary>
    /// <param name="leftVelocity">左モーター強度(0.0f~1.0f)</param>
    /// <param name="rightVelocity">右モーター強度(0.0f~1.0f)</param>
    void SetVibration(float leftVelocity, float rightVelocity);
};

}; // KetaEngine
