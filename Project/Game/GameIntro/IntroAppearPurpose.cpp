#include "IntroAppearPurpose.h"
#include "Frame/Frame.h"
#include <imgui.h>

void IntroAppearPurpose::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // スプライト初期化
    for (size_t i = 0; i < sprites_.size(); ++i) {
        sprites_[i].reset(Sprite::Create("PurposeUI/gamePurposeNo" + std::to_string(i+1) + ".png"));
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

    // 　サイドUIのAppear演出
    for (size_t i = 0; i < spriteVariable_.sideAppearEase.size(); ++i) {
        spriteVariable_.sideAppearEase[i]->Update(Frame::DeltaTime());
        sprites_[i]->transform_.pos.x = spriteVariable_.sideAppearPosX[i];
    }

    if (!spriteVariable_.isBackSideUI) {
        return;
    }

    // 　サイドUIのBack演出
    for (size_t i = 0; i < spriteVariable_.sideBackEase.size(); ++i) {
        spriteVariable_.sideBackEase[i]->Update(Frame::DeltaTime());
        sprites_[i]->transform_.pos.x = spriteVariable_.sideBackPosX[i];
    }

    // centerScale演出
    spriteVariable_.centerAppearEase->Update(Frame::DeltaTime());
    sprites_[CENTER]->transform_.scale = spriteVariable_.centerScale;
   
    //次のステップ
    if (spriteVariable_.centerAppearEase->IsFinished() && spriteVariable_.sideBackEase[LEFT]->IsFinished()) {
        step_ = Step::FINISHWAIT;
    }
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
    for (size_t i = 0; i < spriteVariable_.sideAppearEase.size(); ++i) {
        spriteVariable_.sideAppearEase[i] = std::make_unique<Easing<float>>();
        spriteVariable_.sideBackEase[i]   = std::make_unique<Easing<float>>();
    }
    spriteVariable_.centerAppearEase = std::make_unique<Easing<Vector2>>();

    // サイドUI出現Easing
    spriteVariable_.sideAppearEase[LEFT]->Init("leftAppearPosX", "leftAppearPosX.json");
    spriteVariable_.sideAppearEase[RIGHT]->Init("RightAppearPosX", "RightAppearPosX.json");

    // 元に戻るサイドUIEasing
    spriteVariable_.sideBackEase[LEFT]->Init("LeftBackPosX", "LeftBackPosX.json");
    spriteVariable_.sideBackEase[RIGHT]->Init("RightBackPosX", "RightBackPosX.json");

    // CenterEasing
    spriteVariable_.centerAppearEase->Init("CenterAppearScale", "CenterAppearScale.json");

    // 適応値、スタート値セット(サイドUI出現Easing)
    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideAppearEase.size()); ++i) {
        spriteVariable_.sideAppearEase[i]->SetAdaptValue(&spriteVariable_.sideAppearPosX[i]);
        spriteVariable_.sideAppearEase[i]->SetStartValue(sprites_[i]->GetStartParameter().position_.x);
        spriteVariable_.sideAppearEase[i]->Reset();
        spriteVariable_.sideAppearEase[i]->SetOnFinishCallback([this] {
            spriteVariable_.isBackSideUI = true;
        });
    }

    // 適応値、スタート値セット(元に戻るサイドUIEasing)
    for (size_t i = 0; i < static_cast<size_t>(spriteVariable_.sideBackEase.size()); ++i) {
        spriteVariable_.sideBackEase[i]->SetAdaptValue(&spriteVariable_.sideBackPosX[i]);
        spriteVariable_.sideBackEase[i]->SetStartValue(spriteVariable_.sideAppearEase[i]->GetEndValue());
        spriteVariable_.sideBackEase[i]->SetStartValue(sprites_[i]->GetStartParameter().position_.x);
        spriteVariable_.sideBackEase[i]->Reset();
    }

    // 適応値、スタート値セット(CenterEasing)
    spriteVariable_.centerAppearEase->SetAdaptValue(&spriteVariable_.centerScale);
    spriteVariable_.centerAppearEase->SetStartValue(Vector2::ZeroVector());
    spriteVariable_.centerAppearEase->Reset();
}