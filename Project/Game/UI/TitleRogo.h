#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

#include "Easing/Easing.h"

/// <summary>
/// タイトルロゴ管理クラス
/// </summary>
class TitleRogo {
public:
    // 初期化、更新
    void Init();
    void Update();

    void Draw();      //< 描画
    void EasingSet(); //< イージングの設定
private:
    enum class Step {
        PUNCH,
        RUSH,
        BOTTON,
        END,
    };

private:
    Step step_;
    // 座標
    Vector2 punchPosition_;
    Vector2 rushPosition_;
    Vector2 bottonPosition_;
    // サイズ
    Vector2 size_;
    // スプライト
    std::unique_ptr<Sprite> punchSprite_;
    std::unique_ptr<Sprite> rushSprite_;
    std::unique_ptr<Sprite> bottomSprite_;

    Easing<Vector2> punchEase_;
    Easing<Vector2> rushEase_;
    Easing<Vector2> bottomEase_;

    Vector2 punchSpriteScale_;
    Vector2 rushSpriteScale_;
    Vector2 bottomSpriteScale_;
};