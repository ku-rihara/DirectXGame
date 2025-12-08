#include "TimeScaleController.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void TimeScaleController::Init() {
    AllLoadFile();
    isTimeScaleActive_ = false;
    currentTimer_      = 0.0f;
    targetDuration_    = 0.0f;
}

void TimeScaleController::Update(float deltaTime) {
    if (isTimeScaleActive_) {
        currentTimer_ += deltaTime;

        // 時間が経過したらTimeScaleを戻す
        if (currentTimer_ >= targetDuration_) {
            StopTimeScale();
        }
    }
}

void TimeScaleController::PlayTimeScale(const std::string& timeScaleName) {
    TimeScaleData* timeScale = GetTimeScaleByName(timeScaleName);
    if (timeScale) {
        Frame::SetTimeScale(timeScale->GetTimeScale());

        // 時間管理開始
        isTimeScaleActive_ = true;
        currentTimer_      = 0.0f;
        targetDuration_    = timeScale->GetDuration();
    }
}

void TimeScaleController::StopTimeScale() {
    Frame::SetTimeScale(1.0f);
    isTimeScaleActive_ = false;
    currentTimer_      = 0.0f;
    targetDuration_    = 0.0f;
}

void TimeScaleController::SetTimeScaleImmediate(float timeScale) {
    Frame::SetTimeScale(timeScale);
}

float TimeScaleController::GetCurrentTimeScale() const {
    return Frame::GetTimeScale();
}

bool TimeScaleController::IsTimeScaleActive() const {
    return isTimeScaleActive_;
}

void TimeScaleController::AllLoadFile() {
    // TimeScaleのフォルダ内のすべてのファイルを検索
    std::string folderPath = GlobalParameter::GetInstance()->GetDirectoryPath() + "TimeScale/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // TimeScaleをクリア
        timeScales_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto timeScale = std::make_unique<TimeScaleData>();
                timeScale->Init(fileName);
                timeScale->LoadData(); // Load
                timeScales_.push_back(std::move(timeScale));
            }
        }
    }
}

void TimeScaleController::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("TimeScale Manager")) {

        // 新規追加
        ImGui::InputText("New TimeScale Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add TimeScale")) {
            if (strlen(nameBuffer_) > 0) {
                AddTimeScale(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        // 現在のTimeScale表示
        ImGui::Text("Current TimeScale: %.3f", GetCurrentTimeScale());

        // タイマー情報表示
        if (IsTimeScaleActive()) {
            float remainingTime = targetDuration_ - currentTimer_;
            ImGui::Text("Active Timer: %.2f / %.2f (%.2fs remaining)",
                currentTimer_, targetDuration_, remainingTime);
            ImGui::ProgressBar(currentTimer_ / targetDuration_, ImVec2(0.0f, 0.0f));
        } else {
            ImGui::Text("Status: Inactive");
        }

        ImGui::Separator();

        // TimeScaleリスト表示
        ImGui::Text("TimeScales (%d):", static_cast<int>(timeScales_.size()));
        for (int i = 0; i < static_cast<int>(timeScales_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = timeScales_[i]->GetGroupName() + " (Scale: " + std::to_string(timeScales_[i]->GetTimeScale()) + ", Duration: " + std::to_string(timeScales_[i]->GetDuration()) + "s)";

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            if (ImGui::Button("Play")) {
                PlayTimeScale(timeScales_[i]->GetGroupName());
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたTimeScaleの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(timeScales_.size())) {
            timeScales_[selectedIndex_]->AdjustParam();
        }

        ImGui::Separator();
        ImGui::Text("File Operations:");

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load All TimeScales")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All TimeScales loaded successfully.", "TimeScale Player", 0);
        }
        ImGui::PopStyleColor(3);

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save All TimeScales")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All TimeScales saved successfully.", "TimeScale Player", 0);
        }
        ImGui::PopStyleColor(3);
    }
#endif
}

void TimeScaleController::AddTimeScale(const std::string& timeScaleName) {
    auto timeScale = std::make_unique<TimeScaleData>();
    timeScale->Init(timeScaleName);
    timeScales_.push_back(std::move(timeScale));
    selectedIndex_ = static_cast<int>(timeScales_.size()) - 1;
}

void TimeScaleController::RemoveTimeScale(int index) {
    if (index >= 0 && index < static_cast<int>(timeScales_.size())) {
        timeScales_.erase(timeScales_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !timeScales_.empty()) {
                selectedIndex_ = 0;
            } else if (timeScales_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void TimeScaleController::AllSaveFile() {
    // すべてのTimeScaleデータを保存
    for (auto& timeScale : timeScales_) {
        timeScale->SaveData();
    }
}

TimeScaleData* TimeScaleController::GetTimeScaleByName(const std::string& name) {
    auto it = std::find_if(timeScales_.begin(), timeScales_.end(),
        [&name](const std::unique_ptr<TimeScaleData>& timeScale) {
            return timeScale->GetGroupName() == name;
        });

    if (it != timeScales_.end()) {
        return it->get();
    }
    return nullptr;
}