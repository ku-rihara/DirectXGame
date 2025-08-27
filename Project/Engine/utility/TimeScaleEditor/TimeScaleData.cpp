#include "TimeScaleData.h"
#include <imgui.h>
#include <Windows.h>

void TimeScaleData::Init(const std::string& timeScaleName) {
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = timeScaleName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    globalParameter_->ClearBindingsForGroup(groupName_);
    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void TimeScaleData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void TimeScaleData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void TimeScaleData::BindParams() {
    globalParameter_->Bind(groupName_, "timeScale", &timeScale_);
    globalParameter_->Bind(groupName_, "duration", &duration_);
}

void TimeScaleData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("TimeScale Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // TimeScale調整
        ImGui::DragFloat("TimeScale", &timeScale_, 0.001f);
        ImGui::DragFloat("Duration", &duration_, 0.001f);

        ImGui::Separator();

        // セーブ・ロード
        if (ImGui::Button("Load Data")) {
            LoadData();
            MessageBoxA(nullptr, "TimeScale data loaded successfully.", "TimeScale Data", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Data")) {
            SaveData();
            MessageBoxA(nullptr, "TimeScale data saved successfully.", "TimeScale Data", 0);
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}