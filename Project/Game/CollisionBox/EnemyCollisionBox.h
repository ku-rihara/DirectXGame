#pragma once

#include "BaseAABBCollisionBox.h"
#include <array>

/// <summary>
/// 敵用コリジョンボックスクラス
/// </summary>
class EnemyCollisionBox : public BaseAABBCollisionBox {
public:
   
    // 初期化、更新、描画
    void Init() override;
    void Update(const float& timeSpeed) override;
  
    /// <summary>
    /// 衝突中コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 衝突開始時コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;

public:
    ///------------------------------------------------------------------------------
    /// getter method
    ///------------------------------------------------------------------------------

    Vector3 GetCollisionPos() const override;

    ///------------------------------------------------------------------------------
    /// setter method
    ///------------------------------------------------------------------------------
};