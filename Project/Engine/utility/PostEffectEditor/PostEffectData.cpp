#include "PostEffectData.h"
#include "PostEffect/PostEffectRenderer.h"
#include <imgui.h>
#include <Windows.h>

void PostEffectData::Init(const std::string& postEffectName) {
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = postEffectName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    if (!globalParameter_->HasBindings(groupName_)) {
        BindParams();
    }

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void PostEffectData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void PostEffectData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void PostEffectData::BindParams() {
    globalParameter_->Bind(groupName_, "postEffectModeIndex", &postEffectModeIndex_);
    globalParameter_->Bind(groupName_, "duration", &duration_);
}

void PostEffectData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("PostEffect Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // PostEffectモード選択
        const char* modeNames[] = {"None", "Gray", "Vignette", "Gaus", "BoxFilter", "RadialBlur", "RandomNoize", "Dissolve", "Outline", "LuminanceOutline"};
        if (ImGui::Combo("PostEffect Mode", &postEffectModeIndex_, modeNames, IM_ARRAYSIZE(modeNames))) {
            // モード変更時の処理
        }

        // 基本パラメータ
        ImGui::DragFloat("Duration", &duration_, 0.01f, 0.1f, 10.0f);

        ImGui::Separator();

        // セーブ・ロード
        if (ImGui::Button("Load Data")) {
            LoadData();
            MessageBoxA(nullptr, "PostEffect data loaded successfully.", "PostEffect Data", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Data")) {
            SaveData();
            MessageBoxA(nullptr, "PostEffect data saved successfully.", "PostEffect Data", 0);
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}