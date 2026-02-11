#include "EnemyHPBar.h"
#include "base/TextureManager.h"
#include "MathFunction.h"

void EnemyHPBar::Init(float hpMax) {

    // スプライト生成
    sprite_.reset(KetaEngine::Sprite::Create("boss_Hpbar01.dds", false));
    hpSprite_.reset(KetaEngine::Sprite::Create("boss_Hpbar02.dds", false));

    hpSprite_->SetAnchorPoint(Vector2::ZeroVector());
    sprite_->SetAnchorPoint(Vector2::ZeroVector());
    // レイヤー番号セット
    hpSprite_->SetLayerNum(1);
    sprite_->SetLayerNum(2);

    hpMax_ = hpMax;
}

void EnemyHPBar::Update(float hp) {

    // positionセット
    hpSprite_->transform_.pos = position_;

    // hpMaxを1としてスケールをセット
    float currentScale = Lerp(0.0f, 1.0f, hp / hpMax_);

    hpSprite_->transform_.scale.x = currentScale;

    sprite_->transform_.pos = position_;

    // isDrawセット
    sprite_->SetIsDraw(false);
    hpSprite_->SetIsDraw(false);
}
