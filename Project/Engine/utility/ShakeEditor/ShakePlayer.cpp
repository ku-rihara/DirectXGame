#include "ShakePlayer.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void ShakePlayer::Init() {
    AllLoadFile();
    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakePlayer::Update(float deltaTime) {
    // すべてのシェイクを更新
    UpdateShakes(deltaTime);

    // 合成シェイクオフセットを計算
    UpdateTotalShakeOffset();

    // プレビューオブジェクト更新
    if (preViewObj_) {
        preViewObj_->transform_.translation_ = totalShakeOffset_;
    }
}

void ShakePlayer::UpdateShakes(float deltaTime) {
    for (auto& shake : shakes_) {
        shake->Update(deltaTime);
    }
}

void ShakePlayer::UpdateTotalShakeOffset() {
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

    // すべての再生中シェイクを合成
    for (const auto& shake : shakes_) {
        if (shake->IsPlaying()) {
            Vector3 offset = shake->GetShakeOffset();
            totalShakeOffset_.x += offset.x;
            totalShakeOffset_.y += offset.y;
            totalShakeOffset_.z += offset.z;
        }
    }
}

void ShakePlayer::AllLoadFile() {
    // ShakeEditorのShakeDataフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/ShakeEditor/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のシェイクをクリア
        shakes_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto shake = std::make_unique<ShakeData>();
                shake->Init(fileName);
                shake->LoadData();
                shakes_.push_back(std::move(shake));
            }
        }
    }
}

void ShakePlayer::AllSaveFile() {
    // すべてのシェイクデータを保存
    for (auto& shake : shakes_) {
        shake->SaveData();
    }
}

void ShakePlayer::AddShake(const std::string& shakeName) {
    auto shake = std::make_unique<ShakeData>();
    shake->Init(shakeName);
    shakes_.push_back(std::move(shake));
    selectedIndex_ = static_cast<int>(shakes_.size()) - 1;
}

void ShakePlayer::RemoveShake(int index) {
    if (index >= 0 && index < static_cast<int>(shakes_.size())) {
        shakes_.erase(shakes_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !shakes_.empty()) {
                selectedIndex_ = 0;
            } else if (shakes_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void ShakePlayer::Play(const std::string& shakeName) {
    // 名前でシェイクを検索して再生
    auto* shake = GetShakeByName(shakeName);
    if (shake) {
        shake->Play();
    }
}

void ShakePlayer::PlaySelectedShake() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        shakes_[selectedIndex_]->Play();
    }
}

void ShakePlayer::StopSelectedShake() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        shakes_[selectedIndex_]->Stop();
    }
}

void ShakePlayer::ResetSelectedShake() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        shakes_[selectedIndex_]->Reset();
    }
}

void ShakePlayer::StopAllShakes() {
    for (auto& shake : shakes_) {
        shake->Stop();
    }
    totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

bool ShakePlayer::IsSelectedShakePlaying() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        return shakes_[selectedIndex_]->IsPlaying();
    }
    return false;
}

bool ShakePlayer::IsSelectedShakeFinished() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        return shakes_[selectedIndex_]->IsFinished();
    }
    return false;
}

bool ShakePlayer::IsAnyShakePlaying() const {
    for (const auto& shake : shakes_) {
        if (shake->IsPlaying()) {
            return true;
        }
    }
    return false;
}

ShakeData* ShakePlayer::GetSelectedShake() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        return shakes_[selectedIndex_].get();
    }
    return nullptr;
}

ShakeData* ShakePlayer::GetShakeByName(const std::string& name) {
    auto it = std::find_if(shakes_.begin(), shakes_.end(),
        [&name](const std::unique_ptr<ShakeData>& shake) {
            return shake->GetGroupName() == name;
        });

    if (it != shakes_.end()) {
        return it->get();
    }
    return nullptr;
}

Vector3 ShakePlayer::GetTotalShakeOffset() const {
    return totalShakeOffset_;
}

Vector3 ShakePlayer::GetSelectedShakeOffset() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        return shakes_[selectedIndex_]->GetShakeOffset();
    }
    return {0.0f, 0.0f, 0.0f};
}

void ShakePlayer::SetSelectedIndex(int index) {
    if (index >= -1 && index < static_cast<int>(shakes_.size())) {
        selectedIndex_ = index;
    }
}

void ShakePlayer::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Shake Player")) {

        // 全体制御
        if (ImGui::Button("Load All Shakes")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All shakes loaded successfully.", "Shake Player", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save All Shakes")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All shakes saved successfully.", "Shake Player", 0);
        }

        ImGui::Separator();

        // 新規追加
        ImGui::InputText("New Shake Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Shake")) {
            if (strlen(nameBuffer_) > 0) {
                AddShake(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // 全体制御
        ImGui::Text("Global Controls:");
        if (ImGui::Button("Stop All Shakes")) {
            StopAllShakes();
        }
        ImGui::SameLine();
        ImGui::Text("Any Playing: %s", IsAnyShakePlaying() ? "Yes" : "No");

        // 合成シェイク値表示
        ImGui::Text("Total Shake Offset: (%.2f, %.2f, %.2f)",
            totalShakeOffset_.x, totalShakeOffset_.y, totalShakeOffset_.z);

        ImGui::Separator();

        // シェイクリスト表示
        ImGui::Text("Shakes (%d):", static_cast<int>(shakes_.size()));
        for (int i = 0; i < static_cast<int>(shakes_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = shakes_[i]->GetGroupName();

            // 再生中のシェイクを強調表示
            if (shakes_[i]->IsPlaying()) {
                labelText += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            } else if (shakes_[i]->IsFinished()) {
                labelText += " [FINISHED]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            if (shakes_[i]->IsPlaying() || shakes_[i]->IsFinished()) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたシェイクの制御
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
            ImGui::SeparatorText("Shake Controls");
            ImGui::PushID("selected_controls");

            // 状態表示
            ImGui::Text("Selected: %s", shakes_[selectedIndex_]->GetGroupName().c_str());
            ImGui::Text("Playing: %s", IsSelectedShakePlaying() ? "Yes" : "No");
            ImGui::Text("Finished: %s", IsSelectedShakeFinished() ? "Yes" : "No");

            // 再生制御ボタン
            if (ImGui::Button("Play")) {
                PlaySelectedShake();
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                StopSelectedShake();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset")) {
                ResetSelectedShake();
            }

            // 選択されたシェイクのオフセット値表示
            Vector3 selectedOffset = GetSelectedShakeOffset();
            ImGui::Text("Selected Shake Offset: (%.2f, %.2f, %.2f)",
                selectedOffset.x, selectedOffset.y, selectedOffset.z);

            ImGui::PopID();
            ImGui::Separator();

            // 選択されたシェイクの詳細編集
            shakes_[selectedIndex_]->AdjustParam();
        }
    }
#endif
}