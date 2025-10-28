#include "TimeScaleData.h"
#include <imgui.h>
#include <Windows.h>

void TimeScaleData::Init(const std::string& timeScaleName) {
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = timeScaleName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    if (!globalParameter_->HasBindings(groupName_)) {
        BindParams();
    }
    
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

        ImGui::Separator();
        ImGui::Text("File Operations:");

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load Data")) {
            LoadData();
            MessageBoxA(nullptr, "TimeScale data loaded successfully.", "TimeScale Data", 0);
        }
        ImGui::PopStyleColor(3);

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save Data")) {
            SaveData();
            MessageBoxA(nullptr, "TimeScale data saved successfully.", "TimeScale Data", 0);
        }
        ImGui::PopStyleColor(3);

        ImGui::PopID();
    }
#endif // _DEBUG
}