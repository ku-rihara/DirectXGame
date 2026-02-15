#include "IntroPurposeCutIn.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include "Frame/Frame.h"
#include <imgui.h>

void IntroPurposeCutIn::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // スプライト初期化
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i].reset(KetaEngine::Sprite::Create("PurposeUI/gamePurposeNo" + std::to_string(i + 1) + ".dds"));
        sprites_[i]->transform_.scale = Vector2::ZeroVector();
    }

    backLineSprite_.reset(KetaEngine::Sprite::Create("PurposeUI/gamePurposeLine.dds"));
    backLineSprite_->transform_.scale = Vector2::ZeroVector();

    // 変数初期化
    phase_            = Phase::Appear;
    isFinish_         = false;
    isAbleEnemySpawn_ = false;

    // 全出現アニメーション一斉開始
    sprites_[LEFT]->PlaySpriteEaseAnimation("leftSideAppear", "IntroPurposeCutIn");
    sprites_[RIGHT]->PlaySpriteEaseAnimation("rightSideAppear", "IntroPurposeCutIn");
    sprites_[CENTER]->PlaySpriteEaseAnimation("centerAppearScale", "IntroPurposeCutIn");
    backLineSprite_->PlaySpriteEaseAnimation("appearScale", "IntroPurposeCutIn");
}

void IntroPurposeCutIn::Update(float playSpeed) {
    BaseGameIntro::Update(playSpeed);

    switch (phase_) {
    case Phase::Appear:
        // 全出現アニメーションが終わったらCloseへ
        if (IsAllAppearFinished()) {
            StartCloseAnimation();
            phase_ = Phase::Close;
        }
        break;

    case Phase::Close:
        // 閉じアニメーションが終わったら完了
        if (IsCloseFinished()) {
            phase_    = Phase::Finish;
            isFinish_ = true;
        }
        break;

    case Phase::Finish:
        break;
    }
}

bool IntroPurposeCutIn::IsAllAppearFinished() const {
    for (size_t i = 0; i < sprites_.size(); ++i) {
        if (!sprites_[i]->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            return false;
        }
    }
    if (!backLineSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
        return false;
    }
    return true;
}

void IntroPurposeCutIn::StartCloseAnimation() {
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i]->PlaySpriteEaseAnimation("closeScale", "IntroPurposeCutIn");
    }
    backLineSprite_->PlaySpriteEaseAnimation("closeScale", "IntroPurposeCutIn");
}

bool IntroPurposeCutIn::IsCloseFinished() const {
    for (size_t i = 0; i < sprites_.size(); ++i) {
        if (!sprites_[i]->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            return false;
        }
    }
    if (!backLineSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
        return false;
    }
    return true;
}

void IntroPurposeCutIn::RegisterParams() {
    BaseGameIntro::RegisterParams();
}

void IntroPurposeCutIn::AdjustParam() {
    BaseGameIntro::AdjustParam();
}

void IntroPurposeCutIn::AdjustUniqueParam() {
}
