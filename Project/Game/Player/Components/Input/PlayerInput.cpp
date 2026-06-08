#include "PlayerInput.h"
#include "input/Input.h"

Vector3 PlayerInput::GetMoveDirection() const {
    
    // inputの取得
    Vector3 direction         = {0.0f, 0.0f, 0.0f};
    KetaEngine::Input* input = KetaEngine::Input::GetInstance();

    // キーボード入力
    if (input->PushKey(KeyboardKey::W)) { direction.z += 1.0f; }
    if (input->PushKey(KeyboardKey::S)) { direction.z -= 1.0f; }
    if (input->PushKey(KeyboardKey::A)) { direction.x -= 1.0f; }
    if (input->PushKey(KeyboardKey::D)) { direction.x += 1.0f; }

    // ゲームパッド入力
    Vector2 stickInput = KetaEngine::Input::GetPadStick(0, 0);

    // スティック入力を移動ベクトルに加算
    direction.x += stickInput.x;
    direction.z += stickInput.y;

    // 向きを返す
    return direction;
}

/// ===========================================================
///  移動しているか
/// ===========================================================
bool PlayerInput::IsMoving() const {
    return GetMoveDirection().Length() > kMoveThreshold;
}

/// ===========================================================
///  ダッシュ入力
/// ===========================================================
bool PlayerInput::IsDashInput() const {
    return KetaEngine::Input::IsPressPad(0, GamepadButton::LB);
}

/// ===========================================================
///  ジャンプキーが押されているか - キーボード
/// ===========================================================
bool PlayerInput::IsJumpKeyPressed() const {
    return KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::J);
}

/// ===========================================================
/// ジャンプキーが押されているか - トリガー
/// ===========================================================
bool PlayerInput::IsJumpKeyTriggered() const {
    return KetaEngine::Input::GetInstance()->TriggerKey(KeyboardKey::J);
}

/// ===========================================================
/// ジャンプキーが押されているか - パッド
/// ===========================================================
bool PlayerInput::IsJumpPadTriggered() const {
    return KetaEngine::Input::IsTriggerPad(0, GamepadButton::A);
}
