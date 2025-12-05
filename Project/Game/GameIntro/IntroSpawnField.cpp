#include "IntroSpawnField.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "FireInjectors/FireInjectors.h"
#include "GameCamera/GameCamera.h"
#include "player/Player.h"
#include "UI/HowToOperate.h"
#include <imgui.h>

void IntroSpawnField::Init(const std::string& name) {

    BaseGameIntro::Init(name);

    // 変数初期化
    step_             = Step::START;
    isFinish_         = false;
    isAbleEnemySpawn_ = false;
    currentTime_      = 0.0f;
}

void IntroSpawnField::Update(float playSpeed) {
    BaseGameIntro::Update(playSpeed);
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}


void IntroSpawnField::Start() {
    if (pGameCamera_) {
        pGameCamera_->PlayAnimation("IntroOverLook");
    }
    step_ = Step::WAIT;
}

void IntroSpawnField::Wait() {
    ProcessStep(waitTime_, Step::OBJSPAWN);
}

void IntroSpawnField::ObjSpawn() {
   
    if (pGameBackGroundObject_) {
        pGameBackGroundObject_->Update(playSpeed_);
    }
    if (pFireInjectors_) {
        pFireInjectors_->Spawn();
    }

    ProcessStep(objSpawnTime_, Step::PLAYERSPAWN);
}

void IntroSpawnField::PlayerSpawn() {
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    ProcessStep(playerSpawnTime_, Step::FNISHWAIT, true);
}

void IntroSpawnField::FinishWait() {
    if (pHowToOperate_) {
        pHowToOperate_->ScalingEasing();
    }
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    ProcessStep(finishWaitTime_, Step::FINISH);
}

void IntroSpawnField::Finish() {
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    isFinish_ = true;
}

bool IntroSpawnField::ProcessStep(float limitTime, const Step& nextStep, const bool& enableEnemySpawn) {
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

void IntroSpawnField::RegisterParams() {
    BaseGameIntro::RegisterParams();
    globalParameter_->Regist(groupName_, "waitTime", &waitTime_);
    globalParameter_->Regist(groupName_, "objSpawnTime", &objSpawnTime_);
    globalParameter_->Regist(groupName_, "playerSpawnTime", &playerSpawnTime_);
    globalParameter_->Regist(groupName_, "purposeWaitTime", &finishWaitTime_);
}

void IntroSpawnField::AdjustParam() {
    BaseGameIntro::AdjustParam();
}

void IntroSpawnField::AdjustUniqueParam() {
    ImGui::DragFloat("Wait Time", &waitTime_, 0.01f, 0.0f);
    ImGui::DragFloat("Obj Spawn Time", &objSpawnTime_, 0.01f, 0.0f);
    ImGui::DragFloat("Player Spawn Time", &playerSpawnTime_, 0.01f, 0.0f);
    ImGui::DragFloat("Finish Wait Time", &finishWaitTime_, 0.01f, 0.0f);
 }

void (IntroSpawnField::* IntroSpawnField::spFuncTable_[])() = {
    &IntroSpawnField::Start,
    &IntroSpawnField::Wait,
    &IntroSpawnField::ObjSpawn,
    &IntroSpawnField::PlayerSpawn,
    &IntroSpawnField::FinishWait,
    &IntroSpawnField::Finish,
};