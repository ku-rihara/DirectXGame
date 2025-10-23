#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// 発見時に表示されるスプライトクラス
/// </summary>
class FindSprite {
public:
    // 初期化、更新
    void Init();
    void Update();

    /// <summary>
    /// スケールの設定
    /// </summary>
    /// <param name="scale">設定するスケール</param>
    void SetScale(const Vector2& scale);

private:
    Vector2 position_;
    std::unique_ptr<Sprite> sprite_;

    bool isSpawned_ = false;
    float alpha_;

public:
    // getter
    const bool& GetIsSpawned() const { return isSpawned_; }

    // setter
    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetIsSpawned(const bool& is) { isSpawned_ = is; }
    void SetAlpha(const float& a) { alpha_ = a; }
};