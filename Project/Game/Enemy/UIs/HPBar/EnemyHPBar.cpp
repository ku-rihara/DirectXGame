#include "EnemyHPBar.h"
#include "EnemyHPBarColorConfig.h"
#include "base/TextureManager.h"
#include "MathFunction.h"

void EnemyHPBar::Init(float hpMax) {

    // スプライト生成
    sprite_.reset(KetaEngine::Sprite::Create("Enemy/EnemyHPBar.dds", false));
    hpSprite_.reset(KetaEngine::Sprite::Create("Enemy/EnemyHPGauge.dds", false));

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

    // HP割合に応じた色を適用
    if (colorConfig_) {
        float ratio;
        if (hpMax_ > 0.0f) {
            ratio = hp / hpMax_;
        } else {
            ratio = 1.0f;
        }
        hpSprite_->SetColor(colorConfig_->GetColor(ratio));
    }

    sprite_->transform_.pos = position_;

    // isDrawセット
    sprite_->SetIsDraw(isDraw_);
    hpSprite_->SetIsDraw(isDraw_);
}
