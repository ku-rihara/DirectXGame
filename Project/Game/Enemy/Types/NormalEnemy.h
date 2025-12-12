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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="spawnPos">スポーン位置</param>
    void Init(const Vector3& spownPos) override;

    // 初期化、更新
    void Update() override;

    /// <summary>
    /// スプライト表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;

    void SpawnRenditionInit() override; //< スポーン演出初期化

private:
private:
};