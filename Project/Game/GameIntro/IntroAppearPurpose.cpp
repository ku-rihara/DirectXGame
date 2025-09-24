#include "IntroAppearPurpose.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "FireInjectors/FireInjectors.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "player/Player.h"
#include "UI/HowToOperate.h"
#include <imgui.h>

void IntroAppearPurpose::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // MovieLine 初期化
  

    // 変数初期化
    step_             = Step::APPEARWAIT;
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
void IntroAppearPurpose::APPEARWAIT() {
}
void IntroAppearPurpose::APPEAR() {
}
void IntroAppearPurpose::DISAPPEARWAIT() {
}
void IntroAppearPurpose::DISAPPEAR() {
}
void IntroAppearPurpose::FinishWait() {
}
void IntroAppearPurpose::Finish() {
}

bool IntroAppearPurpose::ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn) {
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

}

void IntroAppearPurpose::AdjustParam() {
    BaseGameIntro::AdjustParam();
}

void IntroAppearPurpose::AdjustUniqueParam() {
    ImGui::DragFloat("AppearWait Time", &appearWaitTime_, 0.01f, 0.0f);
  
}

void (IntroAppearPurpose::* IntroAppearPurpose::spFuncTable_[])() = {
    &IntroAppearPurpose::APPEARWAIT,
    &IntroAppearPurpose::APPEAR,
    &IntroAppearPurpose::DISAPPEARWAIT,
    &IntroAppearPurpose::DISAPPEAR,
    &IntroAppearPurpose::FinishWait,
    &IntroAppearPurpose::Finish,
};