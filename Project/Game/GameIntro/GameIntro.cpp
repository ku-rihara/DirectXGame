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

// 最初の待機
void GameIntro::Wait() {
    currentTime_ += Frame::DeltaTime();

    if (currentTime_ >= waitTime_) {
        currentTime_ = 0.0f;
        step_        = IntroStep::OBJSPAWN;
    }
}

void GameIntro::ObjSpawn() {
    currentTime_ += Frame::DeltaTime();

    // obj生成演出
    pGameBackGroundObject_->Update();
    pFireInjectors_->Spawn();

    if (currentTime_ >= objSpawnTime_) {
        step_        = IntroStep::PLAYERSPAWN;
        currentTime_ = 0.0f;
    }
}
void GameIntro::PlayerSpawn() {
    currentTime_ += Frame::DeltaTime();

    pHowToOperate_->ScalingEasing();
    pPlayer_->GameIntroUpdate();

    if (currentTime_ >= playerSpawnTime_) {
        currentTime_ = 0.0f;
        isAbleEnemySpawn_ = true;
        step_        = IntroStep::PURPOSEAPPEAR;
    }
}

void GameIntro::PurposeAppear() {
   
}

void GameIntro::Finish() {
}

///=========================================================
/// バインド
///==========================================================
void GameIntro::BindParams() {
    globalParameter_->Bind(groupName_, "waitTime", &waitTime_);
    globalParameter_->Bind(groupName_, "objSpawnTime", &objSpawnTime_);
    globalParameter_->Bind(groupName_, "playerSpawnTime", &playerSpawnTime_);
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

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (GameIntro::* GameIntro::spFuncTable_[])(){
    &GameIntro::Start,
    &GameIntro::Wait,
    &GameIntro::ObjSpawn,
    &GameIntro::PlayerSpawn,
    &GameIntro::PurposeAppear,
    &GameIntro::Finish,
};
