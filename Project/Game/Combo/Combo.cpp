#include "Combo.h"
#include "Frame/Frame.h"
#include <imgui.h>

void Combo::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    uiController_ = std::make_unique<ComboUIController>();
    uiController_->Init();
}

void Combo::Update() {
    // コンボカウント
    uiController_->Update(comboCount_);
    // コンボタイマーの更新
    ComboTimerDecrement();
    // コンボタイマーによるアルファの調整
    uiController_->AlphaAdaptForTime(comboTime_, comboTimeMax_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void Combo::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::SeparatorText("Combo Parameter");
        ImGui::DragFloat("Combo Time Max", &comboTimeMax_, 0.01f);
        for (int32_t i = 0; i < kComboLevel; ++i) {
            ImGui::InputInt(("LevelUpComboNum" + std::to_string(int(i + 1))).c_str(), &LevelUpNum[i]);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG

    uiController_->AdjustParam();
}

void Combo::RegisterParams() {
    globalParameter_->Regist(groupName_, "comboTimeMax_", &comboTimeMax_);
    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Regist(groupName_, "LevelUpComboNum" + std::to_string(int(i + 1)), &LevelUpNum[i]);
    }
}

void Combo::ComboCountUP() {
    uiController_->ChangeCountUPAnimation();
    comboCount_++;
    comboTime_ = comboTimeMax_;
}

void Combo::LevelUp() {
    if (currentLevel_ >= kComboLevel - 1) {
        return;
    }
    currentLevel_++; // レベルアップ
}

void Combo::ComboTimerDecrement() {
    comboTime_ -= KetaEngine::Frame::DeltaTimeRate();

    //コンボが途切れたらリセット
    if (comboCount_ == 0) {
        return;
    }

    if (comboTime_ < 0.0f && !isReset_) {
        isReset_ = true;
    }
}

void Combo::Reset() {
    comboTime_    = 0.0f;
    comboCount_   = 0;
    currentLevel_ = 0;
    isReset_      = false;
}