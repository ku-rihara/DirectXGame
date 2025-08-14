#pragma once
// Function

// class
#include "BaseEnemy.h"

class StrongEnemy : public BaseEnemy {
public:
    StrongEnemy()           = default;
    ~StrongEnemy() override = default;
    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新、描画
    void Init(const Vector3& spownPos) override;
    void Update() override;
    void SpriteDraw(const ViewProjection& viewProjection) override;

    /// Hpバー処理
    void DisplaySprite(const ViewProjection& viewProjection) override;
    void SpawnRenditionInit() override;

private:
private:
};