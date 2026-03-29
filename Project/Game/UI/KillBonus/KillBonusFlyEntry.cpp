#include "KillBonusFlyEntry.h"

#include "math/MathFunction.h"
#include "math/random.h"

void KillBonusFlyEntry::Init(const Vector2& startPos, const KillBonusFlyParam& param) {
    p0_       = startPos;
    p2_       = param.endPos;
    duration_ = param.duration;

    // 第1制御点：始点付近・符号含めてランダム
    const float randX  = Random::Range(-param.controlXOffsetRange,  param.controlXOffsetRange);
    p1_ = startPos + Vector2{param.controlOffset.x  + randX,  param.controlOffset.y};

    // 第2制御点：終点付近・符号含めてランダム
    const float randX2 = Random::Range(-param.controlXOffsetRange2, param.controlXOffsetRange2);
    p1b_ = p2_ + Vector2{param.controlOffset2.x + randX2, param.controlOffset2.y};

    // t（0→1）イージング初期化
    tEasing_.Init("KillBonusFlyT.json");
    tEasing_.SetMaxTime(duration_);
    tEasing_.SetAdaptValue(&easedT_);
    tEasing_.Reset();

    sprite_.reset(KetaEngine::Sprite::Create(param.spriteTexture, false));
    if (sprite_) {
        sprite_->SetAnchorPoint({0.5f, 0.5f});
        sprite_->transform_.pos   = p0_;
        sprite_->transform_.scale = param.spriteScale;
        sprite_->SetIsDraw(true);
        sprite_->PlaySpriteEaseAnimation("TimerRecoveryUIAppear", "EnemyDeath");
    }

    // トレイル初期化
    trailFollowPos_ = param.worldStartPos;
    trailPlayer_.Init();
    trailPlayer_.SetFollowPosition(&trailFollowPos_);
    trailPlayer_.Play("KillBonusFly", "KillBonus");
}

void KillBonusFlyEntry::Update(float deltaTime) {
    if (isFinished_) {
        return;
    }

    // t イージング更新
    tEasing_.Update(deltaTime);

    // 3次ベジエ曲線（2制御点で滑らかな曲線）
    const Vector2 L01  = Lerp(p0_,  p1_,  easedT_);
    const Vector2 L12  = Lerp(p1_,  p1b_, easedT_);
    const Vector2 L23  = Lerp(p1b_, p2_,  easedT_);
    const Vector2 Q012 = Lerp(L01,  L12,  easedT_);
    const Vector2 Q123 = Lerp(L12,  L23,  easedT_);
    const Vector2 pos  = Lerp(Q012, Q123, easedT_);

    if (sprite_) {
        sprite_->transform_.pos = pos;
    }

    // トレイル更新
    trailPlayer_.Update();

    if (tEasing_.IsFinished()) {
        isFinished_ = true;
        if (sprite_) {
            sprite_->SetIsDraw(false);
        }
        trailPlayer_.StopAndClear();
        if (onReachCallback_) {
            onReachCallback_();
        }
    }
}
