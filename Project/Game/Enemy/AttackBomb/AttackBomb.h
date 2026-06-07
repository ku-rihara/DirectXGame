#pragma once
#include "3d/Object3d/Object3d.h"
#include "Easing/Easing.h"
#include "Vector3.h"
#include <functional>
#include <memory>

/// <summary>
/// ボス専用攻撃ボム1発分
/// </summary>
class AttackBomb {
public:
    AttackBomb()  = default;
    ~AttackBomb() = default;

    // 初期化と更新
    void Init(const Vector3& startPos, const Vector3& targetPos, float flightTime, float arcHeight);
    void Update();

private:

    /// <summary>
    /// イージングの初期化
    /// </summary>
    /// <param name="arcHeight">アークの高さ</param>
    void InitEasing(float arcHeight);

private:

    // スタート位置、ターゲット位置、現在位置
    Vector3 startPos_;
    Vector3 targetPos_;
    Vector3 position_;

    // 飛行時間、経過時間、着地フラグ
    float flightTime_  = 1.5f;
    float elapsedTime_ = 0.0f;
    bool isLanded_     = false;

    // Y方向のイージングとオフセット
    KetaEngine::Easing<float> arcEasing_;
    float arcOffset_ = 0.0f;

    // モデルオブジェクト
    std::unique_ptr<KetaEngine::Object3d> obj3d_;

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
