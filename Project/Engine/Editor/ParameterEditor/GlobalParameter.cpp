#include "GlobalParameter.h"

using namespace KetaEngine;
#include "base/WinApp.h"
#include <fstream>
#include <imgui.h>
#include <iostream>

GlobalParameter* GlobalParameter::GetInstance() {
    static GlobalParameter instance;
    return &instance;
}

void GlobalParameter::CreateGroup(const std::string& groupName) {
    dates_[groupName]; // グループを作成
}

///============================================================================
/// 値セット
///=============================================================================
template <typename T>
void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value) {
    Group& group = dates_[groupName]; // グループ取得または作成

    // 既存のキーが存在するか確認
    if (group.find(key) != group.end()) {
        auto& existingItem = group[key];

        // 値の型が一致していれば更新
        if (std::holds_alternative<T>(existingItem)) {
            existingItem = value;
        } else {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
    } else {
        // 新規の場合、値を追加
        group[key] = value;
    }
}

///============================================================================
/// アイテム追加
///=============================================================================
template <typename T>
void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T defaultValue) {
    Group& group = dates_[groupName];

    // 既存データの確認
    auto it = group.find(key);
    if (it != group.end()) {
        if (std::holds_alternative<T>(it->second)) {
            // 型が一致する場合、値を保持
            defaultValue = std::get<T>(it->second);
        }
    } else {
        // 新規アイテムの追加
        group[key] = defaultValue;
    }
}

///============================================================================
/// 値取得
///=============================================================================
template <typename T>
T GlobalParameter::GetValue(const std::string& groupName, const std::string& key) const {
    auto itGroup = dates_.find(groupName);
    assert(itGroup != dates_.end());

    const Group& group = dates_.at(groupName);
    auto itItem        = group.find(key);
    assert(itItem != group.end());

    return std::get<T>(itItem->second);
}

//==============================================================================
// ファイル保存・読み込み
//==============================================================================

void GlobalParameter::SaveFile(const std::string& groupName, const std::string& folderName) {
    auto itGroup = dates_.find(groupName);
    assert(itGroup != dates_.end());

    json root;
    for (auto& [itemName, item] : itGroup->second) {
        if (std::holds_alternative<int32_t>(item)) {
            root[groupName][itemName] = std::get<int32_t>(item);
        } else if (std::holds_alternative<uint32_t>(item)) {
            root[groupName][itemName] = std::get<uint32_t>(item);
        } else if (std::holds_alternative<float>(item)) {
            root[groupName][itemName] = std::get<float>(item);
        } else if (std::holds_alternative<Vector2>(item)) {
            Vector2 value             = std::get<Vector2>(item);
            root[groupName][itemName] = json::array({value.x, value.y});
        } else if (std::holds_alternative<Vector3>(item)) {
            Vector3 value             = std::get<Vector3>(item);
            root[groupName][itemName] = json::array({value.x, value.y, value.z});
        } else if (std::holds_alternative<Vector4>(item)) {
            Vector4 value             = std::get<Vector4>(item);
            root[groupName][itemName] = json::array({value.x, value.y, value.z, value.w});
        } else if (std::holds_alternative<bool>(item)) {
            root[groupName][itemName] = std::get<bool>(item);
        } else if (std::holds_alternative<std::string>(item)) {
            root[groupName][itemName] = std::get<std::string>(item);
        }
    }

    std::filesystem::path dir(kDirectoryPath);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    // フォルダ名が指定されている場合はサブディレクトリを作成
    std::string fullDir = kDirectoryPath + folderName;
    if (!folderName.empty()) {
        std::filesystem::path subDir(fullDir);
        if (!std::filesystem::exists(subDir)) {
            std::filesystem::create_directories(subDir);
        }
        fullDir += "/";
    } else if (!folderName.empty()) {
        fullDir += "/";
    }

    std::string filePath = fullDir + groupName + ".json";
    std::ofstream ofs(filePath);
    if (ofs.fail()) {
        std::string message = "Failed to open data file for write.";
        MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
        assert(0);
        return;
    }
    ofs << std::setw(4) << root << std::endl;
    ofs.close();
}

void GlobalParameter::LoadFiles() {
    std::filesystem::path dir(kDirectoryPath);
    if (!std::filesystem::exists(dir))
        return;

    std::error_code ec;

    // kDirectoryPath 直下のJSONファイルを処理
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        if (ec) {
            std::cerr << "Error accessing directory: " << ec.message() << std::endl;
            break;
        }

        if (!entry.is_regular_file(ec))
            continue; // ファイルのみを処理

        if (ec)
            continue; // エラーの場合はスキップ

        const std::filesystem::path& filePath = entry.path();

        // .json ファイルのみを対象
        if (filePath.extension() != ".json")
            continue;

        // フォルダ名は空文字列
        std::string folderName = "";

        // 拡張子を除いたファイル名を取得
        std::string fileName = filePath.stem().string();

        LoadFile(fileName, folderName);
    }

    // kDirectoryPath 直下のフォルダを処理
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        if (ec) {
            std::cerr << "Error accessing directory: " << ec.message() << std::endl;
            break;
        }

        if (!entry.is_directory(ec))
            continue; // フォルダのみを処理

        if (ec)
            continue; // エラーの場合はスキップ

        std::filesystem::path folderPath = entry.path();
        std::string folderName           = folderPath.filename().string();

        // 各フォルダ内のJSONファイルを処理
        std::error_code folder_ec;
        for (const auto& fileEntry : std::filesystem::directory_iterator(folderPath, folder_ec)) {
            if (folder_ec) {
                std::cerr << "Error accessing folder " << folderName << ": " << folder_ec.message() << std::endl;
                break; // このフォルダをスキップして次に進む
            }

            if (!fileEntry.is_regular_file(folder_ec))
                continue; // ファイルのみを処理

            if (folder_ec)
                continue; // エラーの場合はスキップ

            const std::filesystem::path& filePath = fileEntry.path();

            // .json ファイルのみを対象
            if (filePath.extension() != ".json")
                continue;

            // 拡張子を除いたファイル名を取得
            std::string fileName = filePath.stem().string();

            LoadFile(fileName, folderName);
        }
    }
}

void GlobalParameter::LoadFile(const std::string& groupName, const std::string& folderName) {
    std::string filePath = kDirectoryPath + folderName;
    if (!folderName.empty()) {
        filePath += "/";
    }
    filePath += groupName + ".json";

    std::ifstream ifs(filePath);
    if (ifs.fail()) {
        std::cerr << "Warning: Could not open file: " << filePath << std::endl;
        return;
    }

    json root;
    ifs >> root;
    if (ifs.fail()) {
        std::cerr << "Warning: Failed to parse JSON in file: " << filePath << std::endl;
        ifs.close();
        return;
    }
    ifs.close();

    // JSONの構造をチェック
    auto itGroup = root.find(groupName);
    if (itGroup == root.end()) {
        std::cerr << "Warning: Group '" << groupName << "' not found in file: " << filePath << std::endl;
        return;
    }

    // データを安全に読み込み
    for (auto itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
        const std::string& itemName = itItem.key();

        // Integer (int32_t)
        if (itItem->is_number_integer()) {
            int32_t value = itItem->get<int32_t>();
            SetValue(groupName, itemName, value);
        }
        // Float (float)
        else if (itItem->is_number_float()) {
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value));
        }
        // Vector2 (Array of 2 elements)
        else if (itItem->is_array() && itItem->size() == 2) {
            Vector2 value = {itItem->at(0), itItem->at(1)};
            SetValue(groupName, itemName, value);
        }
        // Vector3 (Array of 3 elements)
        else if (itItem->is_array() && itItem->size() == 3) {
            Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
            SetValue(groupName, itemName, value);
        }
        // Vector4 (Array of 4 elements)
        else if (itItem->is_array() && itItem->size() == 4) {
            Vector4 value = {itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3)};
            SetValue(groupName, itemName, value);
        }
        // Boolean
        else if (itItem->is_boolean()) {
            bool value = itItem->get<bool>();
            SetValue(groupName, itemName, value);
        }
        // String
        else if (itItem->is_string()) {
            std::string value = itItem->get<std::string>();
            SetValue(groupName, itemName, value);
        }
    }
}

void GlobalParameter::ParamSaveForImGui(const std::string& groupName, const std::string& folderName) {
    // 保存ボタン
    if (ImGui::Button(std::format("Save {}", groupName).c_str())) {
        SaveFile(groupName, folderName);
        // セーブ完了メッセージ
        std::string message = std::format("{}.json saved.", groupName);
        MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
    }
}

void GlobalParameter::ParamLoadForImGui(const std::string& groupName, const std::string& folderName) {
    // ロードボタン
    if (ImGui::Button(std::format("Load {}", groupName).c_str())) {
        LoadFile(groupName, folderName);
        // セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", groupName.c_str());
        SyncParamForGroup(groupName);
    }
}

void GlobalParameter::CopyGroup(const std::string& fromGroup, const std::string& toGroup) {
    auto it = dates_.find(fromGroup);
    if (it == dates_.end())
        return;

    Group& sourceGroup = it->second;
    Group& destGroup   = dates_[toGroup];

    for (const auto& [key, variant] : sourceGroup) {
        // コピー
        if (std::holds_alternative<int32_t>(variant)) {
            destGroup[key] = std::get<int32_t>(variant);
        } else if (std::holds_alternative<uint32_t>(variant)) {
            destGroup[key] = std::get<uint32_t>(variant);
        } else if (std::holds_alternative<float>(variant)) {
            destGroup[key] = std::get<float>(variant);
        } else if (std::holds_alternative<Vector2>(variant)) {
            destGroup[key] = std::get<Vector2>(variant);
        } else if (std::holds_alternative<Vector3>(variant)) {
            destGroup[key] = std::get<Vector3>(variant);
        } else if (std::holds_alternative<Vector4>(variant)) {
            destGroup[key] = std::get<Vector4>(variant);
        } else if (std::holds_alternative<bool>(variant)) {
            destGroup[key] = std::get<bool>(variant);
        } else if (std::holds_alternative<std::string>(variant)) {
            destGroup[key] = std::get<std::string>(variant);
        }
    }
}

template <typename T>
void GlobalParameter::Regist(const std::string& group, const std::string& key, T* variable) {
    // アイテム追加
    AddItem(group, key, *variable);

    BoundItem item;

    // 変数取得
    item.pullVariant = [=]() {
        *variable = GetValue<T>(group, key);
    };

    // 変数セット
    item.pushVariant = [=]() {
        SetValue<T>(group, key, *variable);
    };

    registerParams_[group].emplace_back(std::move(item));
}

void GlobalParameter::SyncAll() {
    for (auto& [group, items] : registerParams_) {
        for (auto& item : items) {
            item.pushVariant();
        }
    }
}

bool GlobalParameter::HasRegisters(const std::string& groupName) const {
    auto it = registerParams_.find(groupName);
    return it != registerParams_.end() && !it->second.empty();
}

void GlobalParameter::SyncParamForGroup(const std::string& group) {
    auto it = registerParams_.find(group);
    if (it != registerParams_.end()) {
        for (auto& item : it->second) {
            item.pullVariant();
        }
    }
}

void GlobalParameter::ClearRegistersForGroup(const std::string& groupName) {
    auto it = registerParams_.find(groupName);
    if (it != registerParams_.end()) {
        it->second.clear();
    }
}

void GlobalParameter::RemoveGroup(const std::string& groupName) {
    // 登録情報をクリア
    auto it = registerParams_.find(groupName);
    if (it != registerParams_.end()) {
        registerParams_.erase(it);
    }

    // グループデータを削除
    auto dataIt = dates_.find(groupName);
    if (dataIt != dates_.end()) {
        dates_.erase(dataIt);
    }
}

void GlobalParameter::ResetAllRegister() {
    registerParams_.clear();
}

bool GlobalParameter::HasGroup(const std::string& groupName) const {
    return dates_.find(groupName) != dates_.end();
}

// テンプレートの明示的インスタンス化
template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value);
template void GlobalParameter::SetValue<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value);
template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value);
template void GlobalParameter::SetValue<Vector2>(const std::string& groupName, const std::string& key, Vector2 value);
template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
template void GlobalParameter::SetValue<Vector4>(const std::string& groupName, const std::string& key, Vector4 value);
template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value);
template void GlobalParameter::SetValue<std::string>(const std::string& groupName, const std::string& key, std::string value);

template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value);
template void GlobalParameter::AddItem<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value);
template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value);
template void GlobalParameter::AddItem<Vector2>(const std::string& groupName, const std::string& key, Vector2 value);
template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
template void GlobalParameter::AddItem<Vector4>(const std::string& groupName, const std::string& key, Vector4 value);
template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value);
template void GlobalParameter::AddItem<std::string>(const std::string& groupName, const std::string& key, std::string value);

template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
template uint32_t GlobalParameter::GetValue<uint32_t>(const std::string& groupName, const std::string& key) const;
template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
template Vector2 GlobalParameter::GetValue<Vector2>(const std::string& groupName, const std::string& key) const;
template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
template Vector4 GlobalParameter::GetValue<Vector4>(const std::string& groupName, const std::string& key) const;
template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;
template std::string GlobalParameter::GetValue<std::string>(const std::string& groupName, const std::string& key) const;

template void GlobalParameter::Regist<int32_t>(const std::string& group, const std::string& key, int32_t* variable);
template void GlobalParameter::Regist<uint32_t>(const std::string& group, const std::string& key, uint32_t* variable);
template void GlobalParameter::Regist<float>(const std::string& group, const std::string& key, float* variable);
template void GlobalParameter::Regist<Vector2>(const std::string& group, const std::string& key, Vector2* variable);
template void GlobalParameter::Regist<Vector3>(const std::string& group, const std::string& key, Vector3* variable);
template void GlobalParameter::Regist<Vector4>(const std::string& group, const std::string& key, Vector4* variable);
template void GlobalParameter::Regist<bool>(const std::string& group, const std::string& key, bool* variable);
template void GlobalParameter::Regist<std::string>(const std::string& group, const std::string& key, std::string* variable);
