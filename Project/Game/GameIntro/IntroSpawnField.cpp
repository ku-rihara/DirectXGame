#include "IntroSpawnField.h"
#include "Frame/Frame.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "FireInjectors/FireInjectors.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "player/Player.h"
#include "UI/HowToOperate.h"
#include <imgui.h>

void IntroSpawnField::Init(const std::string& name) {
    groupName_ = name;

    // Global parameter setup
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // Initialize objects
    movieLine_ = std::make_unique<MovieLine>();
    movieLine_->Init();

    // Initialize state
    step_             = IntroStep::START;
    isFinish_         = false;
    isAbleEnemySpawn_ = false;
    currentTime_      = 0.0f;
}

void IntroSpawnField::Update() {
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}

void IntroSpawnField::Draw() {
    if (movieLine_) {
        movieLine_->Draw();
    }
}

void IntroSpawnField::Start() {
    if (pGameCamera_) {
        pGameCamera_->PlayAnimation("IntroOverLook");
    }
    step_ = IntroStep::WAIT;
}

void IntroSpawnField::Wait() {
    ProcessStep(waitTime_, IntroStep::OBJSPAWN);
}

void IntroSpawnField::ObjSpawn() {
    if (movieLine_) {
        movieLine_->AppearUpdate();
    }
    if (pGameBackGroundObject_) {
        pGameBackGroundObject_->Update();
    }
    if (pFireInjectors_) {
        pFireInjectors_->Spawn();
    }

    ProcessStep(objSpawnTime_, IntroStep::PLAYERSPAWN);
}

void IntroSpawnField::PlayerSpawn() {
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    ProcessStep(playerSpawnTime_, IntroStep::PURPOSWAIT, true);
}

void IntroSpawnField::PurposeWait() {
    if (pHowToOperate_) {
        pHowToOperate_->ScalingEasing();
    }
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    ProcessStep(purposeWaitTime_, IntroStep::PURPOSEAPPEAR);
}

void IntroSpawnField::PurposeAppear() {
    if (pPlayer_) {
        pPlayer_->GameIntroUpdate();
    }
    isFinish_ = true;
}

void IntroSpawnField::Finish() {
    // Cleanup or final actions
}

bool IntroSpawnField::ProcessStep(float limitTime, IntroStep nextStep, bool enableEnemySpawn) {
    currentTime_ += Frame::DeltaTime();

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

void IntroSpawnField::BindParams() {
    globalParameter_->Bind(groupName_, "waitTime", &waitTime_);
    globalParameter_->Bind(groupName_, "objSpawnTime", &objSpawnTime_);
    globalParameter_->Bind(groupName_, "playerSpawnTime", &playerSpawnTime_);
    globalParameter_->Bind(groupName_, "purposeWaitTime", &purposeWaitTime_);
}

void IntroSpawnField::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("Wait Time", &waitTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Obj Spawn Time", &objSpawnTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Player Spawn Time", &playerSpawnTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Purpose Wait Time", &purposeWaitTime_, 0.01f, 0.0f, 10.0f);

        // 
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    if (movieLine_) {
        movieLine_->AdjustParam();
    }
#endif // _DEBUG
}

void (IntroSpawnField::* IntroSpawnField::spFuncTable_[])() = {
    &IntroSpawnField::Start,
    &IntroSpawnField::Wait,
    &IntroSpawnField::ObjSpawn,
    &IntroSpawnField::PlayerSpawn,
    &IntroSpawnField::PurposeWait,
    &IntroSpawnField::PurposeAppear,
    &IntroSpawnField::Finish,
};