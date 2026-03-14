#pragma once
#include "math/Vector3.h"

/// <summary>
/// プレイヤー入力クラス - 入力値の取得を一元管理
/// </summary>
class PlayerInput {
public:
    /// 移動方向取得 (WASD + スティック合算)
    Vector3 GetMoveDirection() const;

    /// 移動中かどうか
    bool IsMoving() const;

    /// ダッシュ入力 (LB)
    bool IsDashInput() const;

    /// ジャンプ入力 - キーボード (J キー押し続け)
    bool IsJumpKeyPressed() const;

    /// ジャンプ入力 - パッド (A ボタントリガー)
    bool IsJumpPadTriggered() const;

private:
    static constexpr float kMoveThreshold = 0.3f;
};
