#include "TitleLogo.h"
#include "base/TextureManager.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include "Frame/Frame.h"
#include <imgui.h>

void TitleLogo::Init() {

    punchSprite_.reset(KetaEngine::Sprite::Create("TitleFontPunch.dds", true));
    rushSprite_.reset(KetaEngine::Sprite::Create("TitleFontRush.dds", true));
    bottomSprite_.reset(KetaEngine::Sprite::Create("StartBottomA.dds", true));

    punchSprite_->transform_.scale  = Vector2::ZeroVector();
    rushSprite_->transform_.scale   = Vector2::ZeroVector();
    bottomSprite_->transform_.scale = Vector2::ZeroVector();

    step_ = Step::NONE;
}

void TitleLogo::Update() {
    switch (step_) {
    case TitleLogo::Step::NONE:
        ///----------------------------------------------------
        /// NONE
        ///----------------------------------------------------

        step_ = Step::PUNCH;
        break;
    case TitleLogo::Step::PUNCH:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------

        step_ = Step::RUSH;
        punchSprite_->PlaySpriteEaseAnimation("punchSpriteScale", "TitleLogo");
        rushSprite_->PlaySpriteEaseAnimation("rushSpriteScale", "TitleLogo");

        break;
    case TitleLogo::Step::RUSH:
        ///----------------------------------------------------
        /// ラッシュ
        ///----------------------------------------------------
        if (rushSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            step_ = Step::BOTTON;
            bottomSprite_->PlaySpriteEaseAnimation("bottomSpriteScale", "TitleLogo");
        }
        break;
    case TitleLogo::Step::BOTTON:
        ///----------------------------------------------------
        /// ボタン
        ///----------------------------------------------------
        if (bottomSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
            step_ = Step::END;
        }
        break;
    case TitleLogo::Step::END:
        ///----------------------------------------------------
        /// 完了
        ///----------------------------------------------------
        break;
    default:
        break;
    }
}
