#include "Editor.h"
#include <fstream>
#include <imgui.h>
#include <cassert>
#include <stdexcept> // 例外処理のためのインクルード

/// math
#include "Vector3.h"
#include "Vector4.h"


template<typename T>
void Editor::SaveToFile(const std::string& filepath, const T& data) const {
    std::ofstream outFile(filepath);
    if (!outFile) {
        throw std::runtime_error("Error: Not open file: " + filepath);  // エラーメッセージを詳細に
    }
    outFile << data.dump(4);
}

template<typename T>
void Editor::LoadFromFile(const std::string& filepath, T& data) const {
    std::ifstream inFile(filepath);
    if (!inFile) {
        throw std::runtime_error("Error: File not found: " + filepath);  // エラーメッセージを詳細に
    }
    inFile >> data;
}

void Editor::Save(const std::string& filepath, const std::function<json()>& serialize) const {
    try {
        json data = serialize();
        SaveToFile(filepath, data);
        // セーブ完了メッセージ
        ImGui::Text("Save Successful: %s", filepath.c_str());
    } catch (const std::exception& e) {
        // セーブ失敗メッセージ
        ImGui::Text("Save Failed: %s", e.what());
    }
}

void Editor::Load(const std::string& filepath, const std::function<void(const json&)>& deserialize) const {
    try {
        json data;
        LoadFromFile(filepath, data);
        deserialize(data);
        // ロード完了メッセージ
        ImGui::Text("Load Successful: %s", filepath.c_str());
    } catch (const std::exception& e) {
        // ロード失敗メッセージ
        ImGui::Text("Load Failed: %s", e.what());
    }
}
