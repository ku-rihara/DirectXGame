#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

class EnemyHPBar {
public:
    EnemyHPBar()  = default;
    ~EnemyHPBar() = default;

    // 初期化、更新、描画
    void Init(const float& hpMax);
    void Update(const float& hp);
    void Draw();

private:

    // スプライト
    std::unique_ptr<Sprite> sprite_;
    std::unique_ptr<Sprite> hpSprite_;

    // parameter
    bool isSpawned_ = false;
    float hpMax_;
    float alpha_;
    Vector2 position_;

public:
    // getter
    bool GetIsSpawned() const { return isSpawned_; }
    // setter
    void SetPosition(Vector2 pos) { position_ = pos; }
    void SetIsSpawned(bool is) { isSpawned_ = is; }
    void SetAlpha(float a) { alpha_ = a; }
};