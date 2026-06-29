#pragma once
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include "Vector3.h"

/// <summary>
/// プレイヤーの移動全般を管理するコンポーネント
/// </summary>
class PlayerMovement {
public:
    void Init(const KetaEngine::ViewProjection* vp);

    // -------------------------------------------------------
    // XZ 慣性移動
    // -------------------------------------------------------

    /// <summary>
    /// 入力・カメラ変換・速度更新・座標適用などの更新
    /// </summary>
    /// <param name="rawInputDir">スティック入力方向</param>
    /// <param name="transform">プレイヤーのTransform</param>
    /// <param name="speed">最大速度</param>
    /// <param name="outObjectiveAngle">進行方向角度</param>
    void Update(const Vector3& rawInputDir, KetaEngine::WorldTransform& transform,
        float speed, float& outObjectiveAngle);

    /// 速度を即座にゼロにする
    void ForceStop();

public:
    // -------------------------------------------------------
    // ジャンプ・落下
    // -------------------------------------------------------

    /// <summary>
    /// ジャンプ処理
    /// </summary>
    void Jump(float& speed, float fallSpeedLimit, float gravity, KetaEngine::WorldTransform& transform, float groundY);

    /// <summary>
    /// 落下の処理
    /// </summary>
    /// <param name="groundY">着地判定 Y 座標</param>
    /// <param name="isJump">true のとき Y 移動をスキップ</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity,
        KetaEngine::WorldTransform& transform, float groundY, bool isJump = false);

    // -------------------------------------------------------
    // フィールド境界制限
    // -------------------------------------------------------

    /// <summary>
    /// プレイヤーをフィールド内に収める
    /// </summary>
    /// <param name="fieldScale">フィールドの半径スケール</param>
    void MoveToLimit(KetaEngine::WorldTransform& transform, const Vector3& fieldScale);

private:
    void UpdateVelocity(const Vector3& targetVelocity, float dt);

private:
    const KetaEngine::ViewProjection* viewProjection_ = nullptr;
    Vector3 velocity_                                 = Vector3::ZeroVector();

    // パラメータ
    float acceleration_; ///< 加速係数
    float deceleration_; ///< 摩擦係数
    float turnBrake_; ///< 方向転換ブレーキ係数

public:
    const Vector3& GetVelocity() const { return velocity_; }
    float GetSpeed() const;
    
    void SetAcceleration(float _acceleratio) { acceleration_ = _acceleratio; }
    void SetDeceleration(float _deceleration) { deceleration_ = _deceleration; }
    void SetTurnBrake(float _deceleration) { turnBrake_ = _deceleration; }
};
