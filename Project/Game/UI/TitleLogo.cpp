#include "TitleLogo.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void TitleLogo::Init() {
  
    punchSprite_.reset(KetaEngine::Sprite::Create("TitleFontPunch.png", true));   
    rushSprite_.reset(KetaEngine::Sprite::Create("TitleFontRush.png", true));  
    bottomSprite_.reset(KetaEngine::Sprite::Create("StartBottomA.png", true));
   

    EasingSet();

    step_ = Step::PUNCH;
}

void TitleLogo::Update() {
    switch (step_) {
    case TitleLogo::Step::PUNCH:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
        punchEase_.Update(KetaEngine::Frame::DeltaTime());
        punchSprite_->transform_.scale = punchSpriteScale_;

        break;
    case TitleLogo::Step::RUSH:
        ///----------------------------------------------------
        /// ラッシュ
        ///----------------------------------------------------
        rushEase_.Update(KetaEngine::Frame::DeltaTime());
        rushSprite_->transform_.scale = rushSpriteScale_;

        break;
    case TitleLogo::Step::BOTTON:
        ///----------------------------------------------------
        /// ボタン
        ///----------------------------------------------------
        bottomEase_.Update(KetaEngine::Frame::DeltaTime());
        bottomSprite_->transform_.scale = bottomSpriteScale_;

        break;
    case TitleLogo::Step::END:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
        break;
    default:
        break;
    }


}

void TitleLogo::EasingSet() {
    punchEase_.Init("punchSpriteScale.json");
    punchEase_.SetAdaptValue(&punchSpriteScale_);
    punchEase_.Reset();

    punchEase_.SetOnFinishCallback([this]() {
        step_ = Step::RUSH;
    });

    rushEase_.Init("rushSpriteScale.json");
    rushEase_.SetAdaptValue(&rushSpriteScale_);
    rushEase_.Reset();

    rushEase_.SetOnFinishCallback([this]() {
        punchEase_.Reset();
        step_ = Step::BOTTON;
    });

    bottomEase_.Init("bottomSpriteScale.json");
    bottomEase_.SetAdaptValue(&bottomSpriteScale_);
    bottomEase_.Reset();

    bottomEase_.SetOnFinishCallback([this]() {
        step_ = Step::END;
    });
}

