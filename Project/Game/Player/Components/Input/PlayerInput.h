#pragma once
#include "math/Vector3.h"

/// <summary>
/// プレイヤーのアクションの入力を管理するクラス
/// </summary>
class PlayerInput {
public:
    /// 移動方向取得 
    Vector3 GetMoveDirection() const;

    /// 移動中かどうか
    bool IsMoving() const;

    /// ダッシュ入力 - パッド
    bool IsDashInput() const;

    /// ジャンプ入力 - キーボード
    bool IsJumpKeyPressed() const;

    /// ジャンプ入力 - キーボード
    bool IsJumpKeyTriggered() const;

    /// ジャンプ入力 - パッド
    bool IsJumpPadTriggered() const;

private:
   const float kMoveThreshold = 0.3f;
};
