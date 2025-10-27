#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

/// <summary>
/// 未発見時に表示されるスプライトクラス
/// </summary>
class NotFindSprite {
public:
    // 初期化、更新、描画
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

    bool isDraw_ = false;
    float alpha_;

public:
    // getter
    bool GetIsDraw() const { return isDraw_; }

    // setter
    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetIsDraw(const bool& is) { isDraw_ = is; }
    void SetAlpha(const float& a) { alpha_ = a; }
};