#include "TitleRogo.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void TitleRogo::Init() {
  
    punchSprite_.reset(Sprite::Create("TitleFontPunch.png", true));
   
    rushSprite_.reset(Sprite::Create("TitleFontRush.png", true));
    
    bottomSprite_.reset(Sprite::Create("StartBottomA.png", true));
   

    EasingSet();

    step_ = Step::PUNCH;
}

void TitleRogo::Update() {
    switch (step_) {
    case TitleRogo::Step::PUNCH:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
        punchEase_.Update(Frame::DeltaTime());
        punchSprite_->transform_.scale = punchSpriteScale_;

        break;
    case TitleRogo::Step::RUSH:
        ///----------------------------------------------------
        /// ラッシュ
        ///----------------------------------------------------
        rushEase_.Update(Frame::DeltaTime());
        rushSprite_->transform_.scale = rushSpriteScale_;

        break;
    case TitleRogo::Step::BOTTON:
        ///----------------------------------------------------
        /// ボタン
        ///----------------------------------------------------
        bottomEase_.Update(Frame::DeltaTime());
        bottomSprite_->transform_.scale = bottomSpriteScale_;

        break;
    case TitleRogo::Step::END:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
        break;
    default:
        break;
    }


}

void TitleRogo::Draw() {

    punchSprite_->Draw();
    rushSprite_->Draw();
    bottomSprite_->Draw();
}

void TitleRogo::EasingSet() {
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

