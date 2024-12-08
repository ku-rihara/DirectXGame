#include "EmitControlPosManager.h"
#include <fstream>
#include <imgui.h>


EmitControlPosManager::EmitControlPosManager(){}

///=====================================================
/// 制御点追加
///=====================================================
void EmitControlPosManager::AddPosition(const Vector3& position) {
    movePosies_.push_back(position);
}

///=====================================================
/// 制御点削除
///=====================================================
void EmitControlPosManager::RemovePosition(size_t index) {
    if (index < movePosies_.size()) {
        movePosies_.erase(movePosies_.begin() + index);
    }
}

const std::vector<Vector3>& EmitControlPosManager::GetPositions() const {
    return movePosies_;
}

///=====================================================
/// セーブ
///=====================================================
void EmitControlPosManager::SaveToFile(const std::string& filename) {
    json root;
    for (const auto& pos : movePosies_) {
        root.push_back({ {"x", pos.x}, {"y", pos.y}, {"z", pos.z} });
    }

    std::ofstream file(dyrectrypath_ + filename+".json", std::ios::out);
    if (file.is_open()) {
        file << root.dump(4); // JSON データを整形して保存
        file.close();
    }
    else {
        ImGui::Text("Failed to save positions to file.");
    }
}

///=====================================================
/// ロード
///=====================================================
void EmitControlPosManager::LoadFromFile(const std::string& filename) {
    std::ifstream file(dyrectrypath_ + filename+".json", std::ios::in);
    if (file.is_open()) {
        json root;
        file >> root; // JSON データを読み込み
        file.close();

        movePosies_.clear();
        for (const auto& position : root) {
            Vector3 pos;
            pos.x = position.at("x").get<float>();
            pos.y = position.at("y").get<float>();
            pos.z = position.at("z").get<float>();
            movePosies_.push_back(pos);
        }
    }
    else {
        ImGui::Text("Failed to load positions from file.");
    }
}

///=====================================================
/// ImGuiでの処理
///=====================================================
void EmitControlPosManager::ImGuiUpdate(const std::string& filename) {
    if (ImGui::CollapsingHeader("Move Position Manager")) {
        // 現在の座標リストを表示
        ImGui::SeparatorText("Current Positions");
        for (size_t i = 0; i < movePosies_.size(); ++i) {
            // 各制御点の表示と調整機能
            ImGui::PushID(static_cast<int>(i)); // 一意のIDを設定
            if (ImGui::DragFloat3("Edit Position", &movePosies_[i].x, 0.1f)) {
                // 座標を調節
                // 変更した場合、必要ならここで何かのイベントを呼び出せる
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                RemovePosition(i);
            }
            ImGui::PopID(); // IDスコープを終了
        }

        // 座標追加用
        ImGui::SeparatorText("Add New Position");
        ImGui::DragFloat3("New Position", &tempAddPosition_.x, 0.1f);
        if (ImGui::Button("Add Position")) {
            AddPosition(tempAddPosition_);
            tempAddPosition_ = { 0.0f, 0.0f, 0.0f }; // 入力用をリセット
        }

        // 保存・ロードボタン
        ImGui::SeparatorText("File Operations");
        if (ImGui::Button("Save Positions")) {
            SaveToFile(filename);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Positions")) {
            LoadFromFile(filename);
        }
    }
}
