#pragma once
#include <list>
#include <memory>
// baseEnemy
#include "BaseEnemy.h"

/// <summary>
/// 通常敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
    NormalEnemy()           = default;
    ~NormalEnemy() override = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    void Init(const Vector3& spownPos) override;
    void Update() override;

    // スポーン演出
    void SpawnRenditionInit() override;

    /// <summary>
    /// スプライト表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;

    /// <summary>
    /// 追跡開始時のアニメーション初期化
    /// </summary>
    void InitChaseAnimation() override;

    /// <summary>
    /// 追跡中のアニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateChaseAnimation(float deltaTime) override;

    /// <summary>
    /// 待機アニメーションにリセット
    /// </summary>
    void ResetToWaitAnimation()override;

private:
    // 追跡アニメーション制御
    enum class ChaseAnimationState {
        NONE,           // アニメーションなし
        PRE_DASH,       // ダッシュ予備動作
        DASHING         // ダッシュ中
    };
    ChaseAnimationState chaseAnimState_;
    bool isPreDashFinished_;
};
