#include "TitleRogo.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void TitleRogo::Init() {
  
    punchSprite_.reset(Sprite::Create("TitleFontPunch.png", true));
    punchSprite_->anchorPoint_ = {0.5f, 0.5f};

    rushSprite_.reset(Sprite::Create("TitleFontRush.png", true));
    rushSprite_->anchorPoint_ = {0.5f, 0.5f};

    bottomSprite_.reset(Sprite::Create("StartBottomA.png", true));
    bottomSprite_->anchorPoint_ = {0.5f, 0.5f};

    /*  punchPosition_  = {696.0f, 137.9f};
      rushPosition_   = {1066.0f, 135.6f};
      bottonPosition_ = {933, 540};

      punchSprite_->SetScale(Vector2(0, 0));
      rushSprite_->SetScale(Vector2(0, 0));
      bottomSprite_->SetScale(Vector2(0, 0));*/

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

    punchSprite_->transform_.pos  = punchPosition_;
    rushSprite_->transform_.pos   = rushPosition_;
    bottomSprite_->transform_.pos = bottonPosition_;
}

void TitleRogo::Draw() {

    punchSprite_->Draw();
    rushSprite_->Draw();
    bottomSprite_->Draw();
}

void TitleRogo::EasingSet() {
    punchEase_.Init("punchSpriteScale");
    punchEase_.ApplyFromJson("punchSpriteScale.json");
    punchEase_.SaveAppliedJsonFileName();
    punchEase_.SetAdaptValue(&punchSpriteScale_);
    punchEase_.Reset();

    punchEase_.SetOnFinishCallback([this]() {
        step_ = Step::RUSH;
    });

    rushEase_.Init("rushSpriteScale");
    rushEase_.ApplyFromJson("rushSpriteScale.json");
    rushEase_.SaveAppliedJsonFileName();
    rushEase_.SetAdaptValue(&rushSpriteScale_);
    rushEase_.Reset();

    rushEase_.SetOnFinishCallback([this]() {
        punchEase_.Reset();
        step_ = Step::BOTTON;
    });

    bottomEase_.Init("bottomSpriteScale");
    bottomEase_.ApplyFromJson("bottomSpriteScale.json");
    bottomEase_.SaveAppliedJsonFileName();
    bottomEase_.SetAdaptValue(&bottomSpriteScale_);
    bottomEase_.Reset();

    bottomEase_.SetOnFinishCallback([this]() {
        step_ = Step::END;
    });
}

void TitleRogo::Debug() {
    ImGui::SeparatorText("SpriteText");
    ImGui::DragFloat2("punchPosition_", &punchPosition_.x, 0.1f);
    ImGui::DragFloat2("rushPosition_", &rushPosition_.x, 0.1f);
    ImGui::DragFloat2("bottonPosition_", &bottonPosition_.x, 0.1f);
}