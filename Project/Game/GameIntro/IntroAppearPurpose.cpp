#include "IntroAppearPurpose.h"
#include <imgui.h>

void IntroAppearPurpose::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // スプライト初期化
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i].reset(Sprite::Create("PurposeUI/gamePurposeNo" + std::to_string(i) + ".png"));
    }

    // Easing 初期化
    EasingInit();

    // 変数初期化
    step_             = Step::SIDEAPPEARWAIT;
    isFinish_         = false;
    isAbleEnemySpawn_ = false;
    currentTime_      = 0.0f;
}

void IntroAppearPurpose::Update(const float& playSpeed) {
    BaseGameIntro::Update(playSpeed);
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}

void IntroAppearPurpose::Draw() {
}

// Step function
void IntroAppearPurpose::SideAppearWait() {
    ProcessStep(appearWaitTime_, Step::SIDEAPPEAR);
}
void IntroAppearPurpose::SideAppear() {
}

void IntroAppearPurpose::FinishWait() {
}
void IntroAppearPurpose::Finish() {
}

bool IntroAppearPurpose::ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn) {
    // タイム加算
    currentTime_ += playSpeed_;

    if (currentTime_ >= limitTime) {
        currentTime_ = 0.0f;
        step_        = nextStep;
        if (enableEnemySpawn) {
            isAbleEnemySpawn_ = true;
        }
        return true;
    }
    return false;
}

void IntroAppearPurpose::BindParams() {
    BaseGameIntro::BindParams();
    globalParameter_->Bind(groupName_, "AppearWaitTime", &appearWaitTime_);
    globalParameter_->Bind(groupName_, "disAppearWaitTime", &disAppearWaitTime_);
    globalParameter_->Bind(groupName_, "finishWaitTime", &finishWaitTime_);
}

void IntroAppearPurpose::AdjustParam() {
    BaseGameIntro::AdjustParam();
}

void IntroAppearPurpose::AdjustUniqueParam() {
    ImGui::DragFloat("AppearWait Time", &appearWaitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("DisAppear Time", &disAppearWaitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("FinishWait Time", &finishWaitTime_, 0.01f, 0.0f);
}

void (IntroAppearPurpose::* IntroAppearPurpose::spFuncTable_[])() = {
    &IntroAppearPurpose::SideAppearWait,
    &IntroAppearPurpose::SideAppear,
    &IntroAppearPurpose::FinishWait,
    &IntroAppearPurpose::Finish,
};

void IntroAppearPurpose::EasingInit() {

    // 生成
    for (size_t i = 0; i < sprites_.size(); ++i) {
        spriteVariable_.sideAppearEase[i] = std::make_unique<Easing<float>>();
        spriteVariable_.sideBackEase[i]   = std::make_unique<Easing<float>>();
    }

    // サイドUI出現Easing
    spriteVariable_.sideAppearEase[LEFT]->Init("UPMovieLineExit", "UPMovieLineExit.json");
    spriteVariable_.sideAppearEase[RIGHT]->Init("UPMovieLineExit", "UPMovieLineExit.json");

    // 元に戻るサイドUIEasing
    spriteVariable_.sideBackEase[LEFT]->Init("UPMovieLineExit", "UPMovieLineExit.json");
    spriteVariable_.sideBackEase[RIGHT]->Init("UPMovieLineExit", "UPMovieLineExit.json");

    // 適応値、スタート値セット
    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideAppearEase.size()); ++i) {
        spriteVariable_.sideAppearEase[i]->SetAdaptValue(&spriteVariable_.sideAppearPosX[i]);
        spriteVariable_.sideAppearEase[i]->SetStartValue(sprites_[i]->GetStartParameter().position_.x);
        spriteVariable_.sideAppearEase[i]->Reset();
    }

    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideBackEase.size()); ++i) {
        spriteVariable_.sideBackEase[i]->SetAdaptValue(&spriteVariable_.sideBackPosX[i]);
        spriteVariable_.sideBackEase[i]->SetStartValue(spriteVariable_.sideAppearEase[i]->gete);
        spriteVariable_.sideBackEase[i]->Reset();
    }
}