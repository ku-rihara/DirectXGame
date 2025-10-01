#pragma once
#include <list>
#include <memory>
// Function

// class
#include "BaseEnemy.h"

/// <summary>
/// 敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
    NormalEnemy()           = default;
    ~NormalEnemy() override = default;
    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新、描画
    void Init(const Vector3& spownPos) override;
    void Update() override;
  
    /// Hpバー処理
    void DisplaySprite(const ViewProjection& viewProjection) override;

    void SpawnRenditionInit() override;

private:
private:
};