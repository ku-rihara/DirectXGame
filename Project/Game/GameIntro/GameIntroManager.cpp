#include "GameIntroManager.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include <algorithm>
#include <imgui.h>

void GameIntroManager::Init() {

    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParam();
    globalParameter_->SyncParamForGroup(groupName_);

    // Initialize all intro sequences
    for (auto& intro : introSequences_) {
        // 生成
        intro->SetHowToOperate(pHowToOperate_);
        intro->SetGameCamera(pGameCamera_);
        intro->SetPlayer(pPlayer_);
        intro->SetFireInjectors(pFireInjectors_);
        intro->SetGameBackGroundObject(pGameBackGroundObject_);

     
    }
    // 初期化
    introSequences_

    currentPlaySpeed_ = 1.0f;
    currentIndex_     = 0;
    isInitialized_    = true;
}

void GameIntroManager::Update() {
    if (!isInitialized_ || introSequences_.empty()) {
        return;
    }

    ProcessInput();

    // 再生スピード
    float playSpeed = Frame::DeltaTime() * currentPlaySpeed_;

    UpdateCurrentIntro();
}

void GameIntroManager::Draw() {
    if (!isInitialized_ || currentIndex_ >= static_cast<int>(introSequences_.size())) {
        return;
    }

    introSequences_[currentIndex_]->Draw();
}

void GameIntroManager::BindParam() {
  
    globalParameter_->Bind(groupName_, "fastSpeed", &fastSpeed_);
}

void GameIntroManager::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("Fast Speed", &fastSpeed_, 0.1f, 1.0f, 10.0f);
       
        // Save/Load parameters
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    for (auto& intro : introSequences_) {
        intro->AdjustParam();
    }

#endif // _DEBUG
}

void GameIntroManager::AddIntroSequence(std::unique_ptr<BaseGameIntro> intro) {
    if (intro) {
        introSequences_.push_back(std::move(intro));
    }
}

bool GameIntroManager::IsAllIntroFinished() const {
    return currentIndex_ >= static_cast<int>(introSequences_.size());
}


void GameIntroManager::ProcessInput() {
    Input* input = Input::GetInstance();

    if (input->TrrigerKey(DIK_SPACE) || input->IsPressPad(XINPUT_GAMEPAD_A)) {
        currentPlaySpeed_ = fastSpeed_;
    } else {
        currentPlaySpeed_ = 1.0f;
    }
}

void GameIntroManager::UpdateCurrentIntro() {
    if (currentIndex_ >= static_cast<int>(introSequences_.size())) {
        return;
    }

    introSequences_[currentIndex_]->Update();

    if (introSequences_[currentIndex_]->GetIsFinish()) {
        MoveToNextIntro();
    }
}

void GameIntroManager::MoveToNextIntro() {
    currentIndex_++;
}

void GameIntroManager::SetHowToOperate(HowToOperate* howToOperate) {
    pHowToOperate_ = howToOperate;
}

void GameIntroManager::SetGameCamera(GameCamera* gameCamera) {
    pGameCamera_ = gameCamera;
}

void GameIntroManager::SetPlayer(Player* player) {
    pPlayer_ = player;
}

void GameIntroManager::SetFireInjectors(FireInjectors* fireInjectors) {
    pFireInjectors_ = fireInjectors;
}

void GameIntroManager::SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) {
    pGameBackGroundObject_ = gameBackGroundObject;
}
