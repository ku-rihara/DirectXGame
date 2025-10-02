#include "EnemyHPBar.h"
#include "base/TextureManager.h"
#include "MathFunction.h"

void EnemyHPBar::Init(const float& hpMax) {

    // スプライト生成
    sprite_.reset(Sprite::Create("boss_Hpbar01.png", false));
    hpSprite_.reset(Sprite::Create("boss_Hpbar02.png", false));

    hpSprite_->anchorPoint_ = Vector2::ZeroVector();
    sprite_->anchorPoint_   = Vector2::ZeroVector();
    // レイヤー番号セット
    hpSprite_->SetLayerNum(1);
    sprite_->SetLayerNum(2);

    hpMax_ = hpMax;
}

void EnemyHPBar::Update(const float& hp) {

    // positionセット
    hpSprite_->transform_.pos = position_;

    // hpMaxを1としてスケールをセット
    float currentScale = Lerp(0.0f, 1.0f, hp / hpMax_);

    hpSprite_->transform_.scale.x = currentScale;

    sprite_->transform_.pos = position_;

    // isDrawセット
    sprite_->SetIsDraw(isDraw_);
    hpSprite_->SetIsDraw(isDraw_);
}
