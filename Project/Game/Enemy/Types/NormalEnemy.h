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
};
