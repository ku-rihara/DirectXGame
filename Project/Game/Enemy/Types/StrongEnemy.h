#pragma once

// baseEnemy
#include "BaseEnemy.h"

/// <summary>
/// 強敵クラス
/// </summary>
class StrongEnemy : public BaseEnemy {
public:
    StrongEnemy()           = default;
    ~StrongEnemy() override = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    void Init(const Vector3& spownPos) override;
    void Update() override;

    // 近づいた時、離れた時の処理
    void OnPlayerApproachAction() override;
    void OnPlayerDistantAction() override;

    // スポーン演出
    void SpawnRenditionInit() override;

    /// <summary>
    /// UIスプライトの表示
    /// </summary>
    /// <param name="viewProjection"></param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;
};
