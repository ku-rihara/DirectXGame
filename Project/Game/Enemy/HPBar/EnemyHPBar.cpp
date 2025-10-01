#include "EnemyHPBar.h"
#include "base/TextureManager.h"
#include "MathFunction.h"

void EnemyHPBar::Init(const float& hpMax) {
  
    // スプライト生成
    sprite_.reset(Sprite::Create("boss_Hpbar01.png", false));
    hpSprite_.reset(Sprite::Create("boss_Hpbar02.png", false));

    hpMax_ = hpMax;

    // サイズセット
    /*sprite_->SetTextureSize(size_);
    hpSprite_->SetTextureSize(size_);*/
}

void EnemyHPBar::Update(const float& hp) {

    // alphaセット
    sprite_->SetAlpha(1.0f);
    hpSprite_->SetAlpha(1.0f);

    // positionセット
    hpSprite_->transform_.pos = position_;

    // hpMaxを1としてスケールをセット
    float currentScale = Lerp(0.0f, hpMax_, hp / hpMax_);

    /*hpSprite_->SetTextureSize(Vector2(float(max(hp,0.0f)), size_.y));*/
    hpSprite_->transform_.scale.x = currentScale;

    sprite_->transform_.pos = position_;

    // isDrawセット
    sprite_->SetIsDraw(isDraw_);
    hpSprite_->SetIsDraw(isDraw_);
   
}
