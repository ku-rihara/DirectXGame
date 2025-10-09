#include "GameIntroManager.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
// step
#include "IntroPurposeCutIn.h"
#include "IntroSpawnField.h"

#include <algorithm>
#include <imgui.h>

void GameIntroManager::Init() {

    // GlobalParameter
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParam();
    globalParameter_->SyncParamForGroup(groupName_);

    // MovieLine 初期化
    movieLine_ = std::make_unique<MovieLine>();
    movieLine_->Init();

    // 初期化
    introSequences_[SpawnField] = std::make_unique<IntroSpawnField>();
    introSequences_[SpawnField]->Init("IntroSpawnField");

    introSequences_[AppearPurpose] = std::make_unique<IntroPurposeCutIn>();
    introSequences_[AppearPurpose]->Init("IntroPurposeCutIn");

    currentIndex_  = 0;
    isInitialized_ = true;
}

void GameIntroManager::Update() {
    if (!isInitialized_ || introSequences_.empty()) {
        return;
    }

    // 早送り入力
    ProcessInput();

    // 再生スピード
    const float playSpeed = Frame::DeltaTime() * currentPlaySpeedRate_;

    // イントロ更新
    UpdateCurrentIntro(playSpeed);

    // MovieLine更新
    MovieLineUpdate();
}

void GameIntroManager::MovieLineUpdate() {

    // 状態遷移の判定
    CheckMovieLineStateTransition();

    // 状態ごとの更新処理
    switch (movieLineState_) {
    case GameIntroManager::MovieLineState::NONE:
        break;
    case GameIntroManager::MovieLineState::APPEAR:
        movieLine_->AppearUpdate();
        break;
    case GameIntroManager::MovieLineState::EXIT:
        movieLine_->ExitUpdate();
        break;
    default:
        break;
    }
}

void GameIntroManager::CheckMovieLineStateTransition() {

    if (currentIndex_ < static_cast<int>(introSequences_.size())) {

        // movieLine状態遷移
        if (currentIndex_ == SpawnField) {
            auto* spawnField = dynamic_cast<IntroSpawnField*>(introSequences_[SpawnField].get());
            if (spawnField) {
                // ObjSpawn段階でMovieLineを表示開始
                if (spawnField->GetCurrentStep() == IntroSpawnField::Step::OBJSPAWN && movieLineState_ == MovieLineState::NONE) {
                    movieLineState_ = MovieLineState::APPEAR;
                }
                // PlayerSpawn段階でMovieLineを退場開始
                else if (spawnField->GetCurrentStep() == IntroSpawnField::Step::FINISH && movieLineState_ == MovieLineState::APPEAR) {
                    movieLineState_ = MovieLineState::EXIT;
                }
            }
        }
    }
}

void GameIntroManager::UIDraw() {

    //
    if (isInitialized_ && currentIndex_ <= static_cast<int>(introSequences_.size())) {
        introSequences_[currentIndex_]->Draw();
    }

    // movieLine
    if (movieLine_) {
        movieLine_->Draw();
    }
}

void GameIntroManager::BindParam() {

    globalParameter_->Bind(groupName_, "fastSpeed", &fastSpeedRate_);
}

void GameIntroManager::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("Fast Speed", &fastSpeedRate_, 0.01f, 1.0f);

        // セーブ、ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    // 各イントロステップのパラメータ編集
    for (auto& intro : introSequences_) {
        intro->AdjustParam();
    }

    // movieLine
    movieLine_->AdjustParam();

#endif // _DEBUG
}

bool GameIntroManager::IsAllIntroFinished() const {
    return currentIndex_ >= static_cast<int>(introSequences_.size());
}

void GameIntroManager::ProcessInput() {
    Input* input = Input::GetInstance();

    if (input->TriggerKey(DIK_F) || input->IsPressPad(0, XINPUT_GAMEPAD_A)) {
        currentPlaySpeedRate_ = fastSpeedRate_;
    } else {
        currentPlaySpeedRate_ = 1.0f;
    }
}

void GameIntroManager::UpdateCurrentIntro(const float& speed) {
    if (currentIndex_ >= static_cast<int>(introSequences_.size())) {
        return;
    }

    introSequences_[currentIndex_]->Update(speed);

    if (introSequences_[currentIndex_]->GetIsFinish()) {
        MoveToNextIntro();
    }
}

void GameIntroManager::MoveToNextIntro() {
    currentIndex_++;
}

const bool& GameIntroManager::GetIsFinishStep(const IntroStep& step) {

    return introSequences_[step]->GetIsFinish();
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

void GameIntroManager::ClassisSet() {
    //  クラスセット
    for (auto& intro : introSequences_) {
        intro->SetHowToOperate(pHowToOperate_);
        intro->SetGameCamera(pGameCamera_);
        intro->SetPlayer(pPlayer_);
        intro->SetFireInjectors(pFireInjectors_);
        intro->SetGameBackGroundObject(pGameBackGroundObject_);
    }
}