#include "gameIntro.h"
#include "BackGroundObject/GameBackGroundObject.h"
#include "FireInjectors/FireInjectors.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "player/Player.h"
#include "UI/HowToOperate.h"
#include <imgui.h>

void GameIntro::Init() {

    // globalParameter
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // obj
    movieLine_ = std::make_unique<MovieLine>();
    movieLine_->Init();

        /* int32_t spriteHandle=

         sprite_.reset(Sprite::Create())*/

        // step
        step_ = IntroStep::START;
}

void GameIntro::Update() {
    (this->*spFuncTable_[static_cast<size_t>(step_)])();
}

void GameIntro::Start() {
    pGameCamera_->PlayAnimation("IntroOverLook");
    step_ = IntroStep::WAIT;
}

void GameIntro::Wait() {
    ProcessStep(waitTime_, IntroStep::OBJSPAWN);
}

void GameIntro::ObjSpawn() {
    movieLine_->AppearUpdate();
    pGameBackGroundObject_->Update();
    pFireInjectors_->Spawn();

    ProcessStep(objSpawnTime_, IntroStep::PLAYERSPAWN);
}

void GameIntro::PlayerSpawn() {
    pPlayer_->GameIntroUpdate();
    ProcessStep(playerSpawnTime_, IntroStep::PURPOSWAIT, true);
}

void GameIntro::PurposeWait() {
    pHowToOperate_->ScalingEasing();
    pPlayer_->GameIntroUpdate();
    ProcessStep(waitTime_, IntroStep::PURPOSEAPPEAR);
}

void GameIntro::PurposeAppear() {
    pPlayer_->GameIntroUpdate();
    isFinish_ = true;
}
void GameIntro::EndUpdate() {
    movieLine_->ExitUpdate();
}
    
void GameIntro::Finish() {
}

void GameIntro::SpriteDraw() {
    movieLine_->Draw();
 }

///=========================================================
/// バインド
///==========================================================
void GameIntro::BindParams() {
    globalParameter_->Bind(groupName_, "waitTime", &waitTime_);
    globalParameter_->Bind(groupName_, "objSpawnTime", &objSpawnTime_);
    globalParameter_->Bind(groupName_, "playerSpawnTime", &playerSpawnTime_);
    globalParameter_->Bind(groupName_, "purposeWaitTime", &purposeWaitTime_);
}

///=========================================================
/// パラメータ調整
///==========================================================
void GameIntro::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("waitTime", &waitTime_, 0.01f);
        ImGui::DragFloat("objSpawnTime", &objSpawnTime_, 0.01f);
        ImGui::DragFloat("playerSpawnTime", &playerSpawnTime_, 0.01f);
        ImGui::DragFloat("purposeWaitTime", &purposeWaitTime_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
    movieLine_->AdjustParam();
#endif // _DEBUG
}

bool GameIntro::ProcessStep(float limitTime, IntroStep nextStep, bool enableEnemySpawn) {
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

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (GameIntro::* GameIntro::spFuncTable_[])(){
    &GameIntro::Start,
    &GameIntro::Wait,
    &GameIntro::ObjSpawn,
    &GameIntro::PlayerSpawn,
    &GameIntro::PurposeWait,
    &GameIntro::PurposeAppear,
    &GameIntro::Finish,
};
