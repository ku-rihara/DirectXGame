#include "ControlPosManager.h"
#include <fstream>
#include <imgui.h>
#include<d3d12.h>

ControlPosManager::ControlPosManager() {}

void ControlPosManager::Init() {

}

void ControlPosManager::Update(const Vector3& Direction) {

    for (size_t i = 0; i < movePosies_.size(); ++i) {
        transforms_[i].translation_ = movePosies_[i] * Direction;
    }

    for (auto& transform : transforms_) {
        transform.UpdateMatrix();
    }
}

///=====================================================
/// 制御点追加
///=====================================================
void ControlPosManager::AddPoint(const Vector3& position) {
    // movePosies に新しい位置を追加
    movePosies_.push_back(position);

    // transformsに新しい WorldTransform を追加
    WorldTransform newTransform;
    newTransform.Init();
    transforms_.push_back(std::move(newTransform));

    std::unique_ptr<Object3d> obj3d;
    obj3d.reset(Object3d::CreateModel("DebugSphere", ".obj"));
    obj3ds_.push_back(std::move(obj3d));
}

///=====================================================
/// 制御点削除
///=====================================================
void ControlPosManager::RemovePoint(size_t index) {
    if (index < movePosies_.size()) {
      

        movePosies_.erase(movePosies_.begin() + index);
        transforms_.erase(transforms_.begin() + index);  
        obj3ds_.erase(obj3ds_.begin() + index);  
       
    }
}

///=====================================================
/// セーブ
///=====================================================
void ControlPosManager::SaveToFile(const std::string& filename) {
    json root;
    for (const auto& pos : movePosies_) {
        root.push_back({ {"x", pos.x}, {"y", pos.y}, {"z", pos.z} });
    }

    std::ofstream file(dyrectrypath_ + filename + ".json", std::ios::out);
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
void ControlPosManager::LoadFromFile(const std::string& filename) {
    std::ifstream file(dyrectrypath_ + filename + ".json", std::ios::in);
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
            AddPoint(pos);
        }
    }
    else {
        ImGui::Text("Failed to load positions from file.");
    }
}

///=====================================================
/// ImGuiでの処理
///=====================================================
void ControlPosManager::ImGuiUpdate(const std::string& filename) {

    // 色を変更
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));      // 通常時の色
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f)); // ホバー時の色
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));  // アクティブ時の色

    // ヘッダーの開始
    if (ImGui::CollapsingHeader("Control Points")) {

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
                RemovePoint(i);
            }
            ImGui::PopID(); // IDスコープを終了
        }

        // 座標追加用
        ImGui::SeparatorText("Add New Position");
        ImGui::DragFloat3("New Position", &tempAddPosition_.x, 0.1f);
        if (ImGui::Button("Add Position")) {
            AddPoint(tempAddPosition_);
            tempAddPosition_ = { 0.0f, 0.0f, 0.0f }; // 入力用をリセット
        }

        // 保存・ロードボタン
        ImGui::SeparatorText("File Operations");
        if (ImGui::Button("Save Positions")) {
            SaveToFile(filename);
            std::string message = std::format("{}.json saved.", filename);
            MessageBoxA(nullptr, message.c_str(), "EmitRailPosition", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Positions")) {
            LoadFromFile(filename);
        }
    }

    // 色のリセットをヘッダー処理終了後に行う
    ImGui::PopStyleColor(3);
}



void ControlPosManager::Draw(const ViewProjection& viewProjection) {
    for (size_t i = 0; i < transforms_.size(); ++i) {
        // 各制御点に対応するObject3dを描画
        if (i < obj3ds_.size()) {
            obj3ds_[i]->Draw(transforms_[i], viewProjection);  // 各制御点の位置を描画
        }
    }
}

std::vector<Vector3> ControlPosManager::GetPositions() const {
    std::vector<Vector3> positions;
 

    for (const auto& transform : transforms_) {
        positions.push_back(transform.translation_);
    }
    return positions;
}

void ControlPosManager::SetParent(WorldTransform* parent) {
    for (auto& transform : transforms_) {
        transform.SetParent(parent);
    }
}