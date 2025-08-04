#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

class EnemyHPBar {
public:
    EnemyHPBar()  = default;
    ~EnemyHPBar() = default;

    // 初期化、更新、描画
    void Init(const Vector2& size);
    void Update(int hp);
    void Draw();

private:

    // スプライト
    std::unique_ptr<Sprite> sprite_;
    std::unique_ptr<Sprite> hpSprite_;

    // parameter
    bool isSpawned_ = false;
    float alpha_;
    Vector2 size_;
    Vector2 position_;

public:
    // getter
    bool GetIsSpawned() const { return isSpawned_; }
    // setter
    void SetPosition(Vector2 pos) { position_ = pos; }
    void SetSize(Vector2 size) { size_ = size; }
    void SetIsSpawned(bool is) { isSpawned_ = is; }
    void SetAlpha(float a) { alpha_ = a; }
};