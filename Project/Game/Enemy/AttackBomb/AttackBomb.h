#pragma once
#include "3d/Object3d/Object3d.h"
#include "Easing/Easing.h"
// Effect
#include "Editor/RibbonTrailEditor/RibbonTrailPlayer.h"
#include "Particle/CPUParticle/ParticlePlayer.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>

/// <summary>
/// ボス専用攻撃ボム1発分
/// </summary>
class AttackBomb {
public:
    AttackBomb()  = default;
    ~AttackBomb() = default;

    struct Param {
        float flightTime   = 1.5f;
        float arcHeight    = 8.0f;
        float rotateZSpeed = 0.0f;
        Vector3 scale;
    };

    // 初期化と更新
    void Init(const Vector3& startPos, const Vector3& targetPos, const Param& param);
    void Update();

    /// トレイルのエミット停止
    void StopTrail();

private:
    void InitEasing(float arcHeight);

private:
    // スタート位置、ターゲット位置、現在位置
    Vector3 startPos_;
    Vector3 targetPos_;
    Vector3 position_;

    Param param_;

    // 経過時間、着地フラグ
    float elapsedTime_ = 0.0f;
    bool isLanded_     = false;

    // Y方向のイージングとオフセット
    KetaEngine::Easing<float> arcEasing_;
    float arcOffset_ = 0.0f;

    // モデルオブジェクト
    std::unique_ptr<KetaEngine::Object3d> obj3d_;

    // エフェクトクラス
    KetaEngine::RibbonTrailPlayer trailPlayer_;
    KetaEngine::ParticlePlayer particlePlayer_;

    // 着地コールバック
    std::function<void()> onLanded_;

public:
    // Getter
    bool IsLanded() const { return isLanded_; }
    const Vector3& GetPosition() const { return position_; }

    // Setter
    void SetOnLandedCallback(std::function<void()> cb) { onLanded_ = std::move(cb); }
    void SetTargetPosition(const Vector3& pos) { targetPos_ = pos; }
};
