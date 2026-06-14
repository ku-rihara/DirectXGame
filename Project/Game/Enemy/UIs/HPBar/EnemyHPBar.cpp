#include "EnemyHPBar.h"
#include "base/TextureManager.h"
#include "EnemyHPBarColorConfig.h"
#include "MathFunction.h"

void EnemyHPBar::Init(float hpMax) {

    // スプライト生成
    barSprite_.reset(KetaEngine::Sprite::Create("Enemy/EnemyHPBar.dds", true));
    hpSprite_.reset(KetaEngine::Sprite::Create("Enemy/EnemyHPGauge.dds", true));

    // 初期スケール
    barSprite_->transform_.scale = Vector2::ZeroVector();
    hpSprite_->transform_.scale  = Vector2::ZeroVector();

    hpMax_ = hpMax;
}

void EnemyHPBar::Show() {
    barSprite_->transform_.scale = Vector2::OneVector();
    hpSprite_->transform_.scale  = Vector2::OneVector();
}

void EnemyHPBar::Update(float hp) {

    // positionセット
    hpSprite_->transform_.pos = position_ + gaugeOffset_;

    // HP割合を計算してゲージ表示範囲をセット
    float ratio = 1.0f;
    if (hpMax_ > 0.0f) {
        ratio = Clamp(hp / hpMax_, 0.0f, 1.0f);
    }
    hpSprite_->SetDisplayRate(ratio);

    // HP割合に応じた色を適用
    if (colorConfig_) {
        hpSprite_->SetColor(colorConfig_->GetColor(ratio));
    }

    barSprite_->transform_.pos = position_;

    // isDrawセット
    barSprite_->SetIsDraw(isDraw_);
    hpSprite_->SetIsDraw(isDraw_);
}
