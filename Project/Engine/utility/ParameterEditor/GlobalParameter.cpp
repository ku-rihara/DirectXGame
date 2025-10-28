#include "GlobalParameter.h"
#include "base/WinApp.h"
#include <fstream>
#include <imgui.h>
#include <iostream>

GlobalParameter* GlobalParameter::GetInstance() {
    static GlobalParameter instance;
    return &instance;
}


void GlobalParameter::CreateGroup(const std::string& groupName, const bool& isVisible) {
    dates_[groupName]; // グループを作成
    visibilityFlags_[groupName] = isVisible; // 表示フラグを設定
}

void GlobalParameter::Update() {
#ifdef _DEBUG
    if (!ImGui::Begin("Global Parameter", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    // 各グループの順番通りに処理
    for (auto& [groupName, group] : dates_) {
        // 表示フラグがオフの場合スキップ
        if (!visibilityFlags_[groupName]) {
            continue;
        }

        // グループタイトルを表示
        if (ImGui::CollapsingHeader(groupName.c_str())) {
            std::string currentSection;

            // 順番通りにアイテムを表示
            for (auto& [itemName, param] : group) {
                auto& [item, drawSettings] = param;

                // セクションタイトルが設定されている場合
                if (!drawSettings.treeNodeLabel.empty()) {
                    if (currentSection != drawSettings.treeNodeLabel) {
                        currentSection = drawSettings.treeNodeLabel;
                        ImGui::SeparatorText(currentSection.c_str()); // セクションヘッダ表示
                    }
                }

                // アイテム描画
                DrawWidget(itemName, item, drawSettings);

                // アイテム間にスペースを追加
                ImGui::Spacing();
            }

            // セーブ・ロード
            ParamSaveForImGui(groupName);
            ParamLoadForImGui(groupName);
        }

        // グループ間に区切りを挿入
        ImGui::Separator();
    }

    ImGui::End();
#endif // _DEBUG
}


void GlobalParameter::DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings) {
    std::visit([&](auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int32_t>) {
            if (drawSettings.widgetType == WidgetType::SliderInt) {
                ImGui::SliderInt(itemName.c_str(), &value, static_cast<int>(drawSettings.minValue), static_cast<int>(drawSettings.maxValue));
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (drawSettings.widgetType == WidgetType::DragFloat) {
                ImGui::DragFloat(itemName.c_str(), &value, 0.1f);
            } else if (drawSettings.widgetType == WidgetType::SlideAngle) {
                // 角度用ウィジェットの描画（SliderAngle使用）
                ImGui::SliderAngle(itemName.c_str(), &value, 0, 360);
            }
        } else if constexpr (std::is_same_v<T, Vector3>) {
            if (drawSettings.widgetType == WidgetType::DragFloat3) {
                ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(&value), 0.1f);
            } else if (drawSettings.widgetType == WidgetType::SlideAngle) {
                // Vector3の各成分ごとにSliderAngleを描画
                if (ImGui::TreeNode((itemName + " (Angle)").c_str())) {
                    ImGui::SliderAngle("X", &value.x, 0, 360);
                    ImGui::SliderAngle("Y", &value.y, 0, 360);
                    ImGui::SliderAngle("Z", &value.z, 0, 360);
                    ImGui::TreePop();
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (drawSettings.widgetType == WidgetType::Checkbox) {
                ImGui::Checkbox(itemName.c_str(), &value);
            }
        } else if constexpr (std::is_same_v<T, Vector4>) {
            if (drawSettings.widgetType == WidgetType::ColorEdit4) {
                ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(&value));
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            // std::stringの場合は表示しない
            if (drawSettings.widgetType == WidgetType::NONE) {
                /*  ImGui::Text(itemName.c_str());*/
            }
        }
    },
        item);
}


///============================================================================
/// 値セット
///=============================================================================
template <typename T>
void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value, WidgetType widgetType) {
    Group& group = dates_[groupName]; // グループ取得または作成

    // 現在のツリーノードがスタックにある場合、そのラベルを取得
    std::string treeNodeLabel = treeNodeStack_.empty() ? "" : treeNodeStack_.top();

    // 既存のキーが存在するか確認
    if (group.find(key) != group.end()) {
        auto& [existingItem, existingSettings] = group[key];

        // 値の型が一致していれば更新
        if (std::holds_alternative<T>(existingItem)) {
            existingItem = value;
        } else {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
       
    } else {
        // 新規の場合、値と設定を追加
        DrawSettings settings;
        settings.widgetType    = widgetType;
        settings.treeNodeLabel = treeNodeLabel;

        group[key] = {value, settings};
    }
}

///============================================================================
/// アイテム追加
///=============================================================================
template <typename T>
void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T defaultValue, WidgetType widgetType) {
    Group& group = dates_[groupName];

    // 既存データの確認
    auto it = group.find(key);
    if (it != group.end()) {
        auto& existingParam = it->second;
        if (std::holds_alternative<T>(existingParam.first)) {
            // 型が一致する場合、値を保持
            defaultValue = std::get<T>(existingParam.first);
        }
        // 描画設定を更新
        existingParam.second.widgetType = widgetType;
        if (!treeNodeStack_.empty()) {
            existingParam.second.treeNodeLabel = treeNodeStack_.top();
        }
    } else {
        // 新規アイテムの追加
        DrawSettings settings;
        settings.widgetType = widgetType;
        if (!treeNodeStack_.empty()) {
            settings.treeNodeLabel = treeNodeStack_.top();
        }
        group[key] = {defaultValue, settings}; // 新規データを追加
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

    return std::get<T>(itItem->second.first);
}

void GlobalParameter::AddSeparatorText(const std::string& nodeName) {
    treeNodeStack_.push(nodeName); // ノード名をスタックに追加
}

void GlobalParameter::AddTreePoP() {
    if (!treeNodeStack_.empty()) {
        treeNodeStack_.pop(); // 現在のノードをスタックから削除
    }
}


//==============================================================================
// ファイル保存・読み込み
//==============================================================================

void GlobalParameter::SaveFile(const std::string& groupName, const std::string& folderName) {
    auto itGroup = dates_.find(groupName);
    assert(itGroup != dates_.end());

    json root;
    for (auto& [itemName, item] : itGroup->second) {
        if (std::holds_alternative<int32_t>(item.first)) {
            root[groupName][itemName] = std::get<int32_t>(item.first);
        } else if (std::holds_alternative<uint32_t>(item.first)) {
            root[groupName][itemName] = std::get<uint32_t>(item.first);
        } else if (std::holds_alternative<float>(item.first)) {
            root[groupName][itemName] = std::get<float>(item.first);
        } else if (std::holds_alternative<Vector2>(item.first)) {
            Vector2 value             = std::get<Vector2>(item.first);
            root[groupName][itemName] = json::array({value.x, value.y});
        } else if (std::holds_alternative<Vector3>(item.first)) {
            Vector3 value             = std::get<Vector3>(item.first);
            root[groupName][itemName] = json::array({value.x, value.y, value.z});
        } else if (std::holds_alternative<Vector4>(item.first)) {
            Vector4 value             = std::get<Vector4>(item.first);
            root[groupName][itemName] = json::array({value.x, value.y, value.z, value.w});
        } else if (std::holds_alternative<bool>(item.first)) {
            root[groupName][itemName] = std::get<bool>(item.first);
        } else if (std::holds_alternative<std::string>(item.first)) {
            root[groupName][itemName] = std::get<std::string>(item.first);
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
     
    }

    json root;
    ifs >> root;
    if (ifs.fail()) {
        std::cerr << "Warning: Failed to parse JSON in file: " << filePath << std::endl;
        ifs.close();
   
    }
    ifs.close();

    // JSONの構造をチェック
    auto itGroup = root.find(groupName);
    if (itGroup == root.end()) {
        std::cerr << "Warning: Group '" << groupName << "' not found in file: " << filePath << std::endl;
     
    }

    // データを読み込み
    for (auto itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
        const std::string& itemName = itItem.key();

        // Integer (int32_t)
        if (itItem->is_number_integer()) {
            int32_t value = itItem->get<int32_t>();
            SetValue(groupName, itemName, value, WidgetType::SliderInt);
        }
        // Float (float)
        else if (itItem->is_number_float()) {
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value), WidgetType::DragFloat);
        }
        // Vector2 (Array of 2 elements)
        else if (itItem->is_array() && itItem->size() == 2) {
            Vector2 value = {itItem->at(0), itItem->at(1)};
            SetValue(groupName, itemName, value, WidgetType::DragFloat2);
        }
        // Vector3 (Array of 3 elements)
        else if (itItem->is_array() && itItem->size() == 3) {
            Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
            SetValue(groupName, itemName, value, WidgetType::DragFloat3);
        }
        // Vector4 (Array of 4 elements)
        else if (itItem->is_array() && itItem->size() == 4) {
            Vector4 value = {itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3)};
            SetValue(groupName, itemName, value, WidgetType::DragFloat4);
        }
        // Boolean
        else if (itItem->is_boolean()) {
            bool value = itItem->get<bool>();
            SetValue(groupName, itemName, value, WidgetType::Checkbox);
        }
        // String
        else if (itItem->is_string()) {
            std::string value = itItem->get<std::string>();
            SetValue(groupName, itemName, value, WidgetType::NONE);
        } 
    }

}
void GlobalParameter::CopyGroup(const std::string& fromGroup, const std::string& toGroup) {
    auto it = dates_.find(fromGroup);
    if (it == dates_.end())
        return;

    Group& sourceGroup = it->second;
    Group& destGroup   = dates_[toGroup]; 

    for (const auto& [key, valueAndSettings] : sourceGroup) {
        const auto& variant  = valueAndSettings.first;
        const auto& settings = valueAndSettings.second;

        // コピー
        if (std::holds_alternative<int32_t>(variant)) {
            destGroup[key] = {std::get<int32_t>(variant), settings};
        } else if (std::holds_alternative<uint32_t>(variant)) {
            destGroup[key] = {std::get<uint32_t>(variant), settings};
        } else if (std::holds_alternative<float>(variant)) {
            destGroup[key] = {std::get<float>(variant), settings};
        } else if (std::holds_alternative<Vector2>(variant)) {
            destGroup[key] = {std::get<Vector2>(variant), settings};
        } else if (std::holds_alternative<Vector3>(variant)) {
            destGroup[key] = {std::get<Vector3>(variant), settings};
        } else if (std::holds_alternative<Vector4>(variant)) {
            destGroup[key] = {std::get<Vector4>(variant), settings};
        } else if (std::holds_alternative<bool>(variant)) {
            destGroup[key] = {std::get<bool>(variant), settings};
        } else if (std::holds_alternative<std::string>(variant)) {
            destGroup[key] = {std::get<std::string>(variant), settings};
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

template <typename T>
void GlobalParameter::Bind(const std::string& group, const std::string& key, T* variable, WidgetType widgetType) {
    // ImGuiで使うためにAddItem
    AddItem(group, key, *variable, widgetType);

    BoundItem item;

    // 変数取得
    item.pullVariant = [=]() {
        *variable = GetValue<T>(group, key);
    };

    // 変数セット
    item.pushVariant = [=]() {
        SetValue<T>(group, key, *variable, widgetType);
    };
   

    bindings_[group].emplace_back(std::move(item));
}

//
void GlobalParameter::SyncAll() {
    for (auto& [group, items] : bindings_) {
        for (auto& item : items) {
            item.pushVariant();
        }
    }
}

bool GlobalParameter::HasBindings(const std::string& groupName) const {
    auto it = bindings_.find(groupName);
    return it != bindings_.end() && !it->second.empty();
}


void GlobalParameter::SyncParamForGroup(const std::string& group) {
    auto it = bindings_.find(group);
    if (it != bindings_.end()) {
        for (auto& item : it->second) {
            item.pullVariant();
        }
    }
}


void GlobalParameter::RemoveGroup(const std::string& groupName) {
    // バインド情報をクリア
    auto it = bindings_.find(groupName);
    if (it != bindings_.end()) {
        bindings_.erase(it);
    }

    // グループデータを削除
    auto dataIt = dates_.find(groupName);
    if (dataIt != dates_.end()) {
        dates_.erase(dataIt);
    }

    // 表示フラグを削除
    auto visIt = visibilityFlags_.find(groupName);
    if (visIt != visibilityFlags_.end()) {
        visibilityFlags_.erase(visIt);
    }
}

void GlobalParameter::ClearBindingsForGroup(const std::string& groupName) {
    auto it = bindings_.find(groupName);
    if (it != bindings_.end()) {
        it->second.clear(); 
    }
}

void GlobalParameter::BindResetAll() {
    bindings_.clear();
}

bool GlobalParameter::HasGroup(const std::string& groupName) const {
    return dates_.find(groupName) != dates_.end();
}

template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value, WidgetType widgetType);
template void GlobalParameter::SetValue<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, WidgetType widgetType);
template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value, WidgetType widgetType);
template void GlobalParameter::SetValue<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, WidgetType widgetType);
template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, WidgetType widgetType);
template void GlobalParameter::SetValue<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, WidgetType widgetType);
template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value, WidgetType widgetType);
template void GlobalParameter::SetValue<std::string>(const std::string& groupName, const std::string& key, std::string value, WidgetType widgetType);

template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value, WidgetType widgetType);
template void GlobalParameter::AddItem<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, WidgetType widgetType);
template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value, WidgetType widgetType);
template void GlobalParameter::AddItem<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, WidgetType widgetType);
template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, WidgetType widgetType);
template void GlobalParameter::AddItem<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, WidgetType widgetType);
template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value, WidgetType widgetType);
template void GlobalParameter::AddItem<std::string>(const std::string& groupName, const std::string& key, std::string value, WidgetType widgetType);

template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
template uint32_t GlobalParameter::GetValue<uint32_t>(const std::string& groupName, const std::string& key) const;
template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
template Vector2 GlobalParameter::GetValue<Vector2>(const std::string& groupName, const std::string& key) const;
template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
template Vector4 GlobalParameter::GetValue<Vector4>(const std::string& groupName, const std::string& key) const;
template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;
template std::string GlobalParameter::GetValue<std::string>(const std::string& groupName, const std::string& key) const;

template void GlobalParameter::Bind<int32_t>(const std::string& group, const std::string& key, int32_t* variable, WidgetType widgetType);
template void GlobalParameter::Bind<uint32_t>(const std::string& group, const std::string& key, uint32_t* variable, WidgetType widgetType);
template void GlobalParameter::Bind<float>(const std::string& group, const std::string& key, float* variable, WidgetType widgetType);
template void GlobalParameter::Bind<Vector2>(const std::string& group, const std::string& key, Vector2* variable, WidgetType widgetType);
template void GlobalParameter::Bind<Vector3>(const std::string& group, const std::string& key, Vector3* variable, WidgetType widgetType);
template void GlobalParameter::Bind<Vector4>(const std::string& group, const std::string& key, Vector4* variable, WidgetType widgetType);
template void GlobalParameter::Bind<bool>(const std::string& group, const std::string& key, bool* variable, WidgetType widgetType);
template void GlobalParameter::Bind<std::string>(const std::string& group, const std::string& key, std::string* variable, WidgetType widgetType);