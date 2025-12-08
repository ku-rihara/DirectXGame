#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// 敵のHPバー表示クラス
/// </summary>
class EnemyHPBar {
public:
    EnemyHPBar()  = default;
    ~EnemyHPBar() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="hpMax">最大HP</param>
    void Init(float hpMax);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="hp">現在のHP</param>
    void Update(float hp);

private:
    std::unique_ptr<Sprite> sprite_;
    std::unique_ptr<Sprite> hpSprite_;

    bool isSpawned_ = false;
    bool isDraw_    = false;
    float hpMax_;
    float alpha_;
    Vector2 position_;

public:
    // getter
    const bool& GetIsSpawned() const { return isSpawned_; }

    // setter
    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetIsSpawned(const bool& is) { isSpawned_ = is; }
    void SetAlpha(float a) { alpha_ = a; }
    void SetIsDraw(const bool& is) { isDraw_ = is; }
};