
#include "GlobalParameter.h"
#include "base/WinApp.h"
#include <fstream>
#include <imgui.h>


GlobalParameter* GlobalParameter::GetInstance() {
    static GlobalParameter instance;
    return &instance;
}

/// =====================================================================================
/// 指定したグループ名のグループを作成。
/// =====================================================================================
void GlobalParameter::CreateGroup(const std::string& groupName) {
    datas_[groupName];
}


/// =====================================================================================
/// デバッグ時の更新処理。
/// 値を操作可能にする。
/// また、保存ボタンを押すと指定グループをファイルに保存。
/// =====================================================================================
void GlobalParameter::Update() {
#ifdef _DEBUG
    if (!ImGui::Begin("Global Parameter", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar()) {
        // 各グループの処理
        for (auto& [groupName, group] : datas_) {
            if (ImGui::BeginMenu(groupName.c_str())) {
                std::string currentTreeNode; // 現在のツリーノード名を追跡

                for (auto& [itemName, param] : group) {
                    auto& [item, drawSettings] = param;

                    // ツリーノードが指定されている場合
                    if (!drawSettings.treeNodeLabel.empty()) {
                        // 異なるツリーノードならば新しいツリーノードを開始
                        if (currentTreeNode != drawSettings.treeNodeLabel) {
                            // 現在のツリーノードを閉じる
                            if (!currentTreeNode.empty()) {
                                ImGui::TreePop();
                            }

                            currentTreeNode = drawSettings.treeNodeLabel;
                            if (!ImGui::TreeNode(currentTreeNode.c_str())) {
                                currentTreeNode.clear();
                                break; // ツリーノードが閉じられた場合、描画をスキップ
                            }
                        }
                    }

                    // アイテム描画
                    DrawWidget(itemName, item, drawSettings);
                }

                // 最後にツリーノードを閉じる
                if (!currentTreeNode.empty()) {
                    ImGui::TreePop();
                }

                // 保存ボタン
                if (ImGui::Button("Save")) {
                    SaveFile(groupName);
                    std::string message = std::format("{}.json saved.", groupName);
                    MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
#endif // _DEBUG
}


void GlobalParameter::DrawGroup(Group& group) {
    group;
    //std::string currentTreeNodeLabel;
    //bool treeNodeOpen = false;

    //for (auto& [key, parameter] : group) {
    //    auto& [value, settings] = parameter;

    //    // ツリーノードの管理
    //    if (settings.treeNodeLabel != currentTreeNodeLabel) {
    //        if (treeNodeOpen) {
    //            ImGui::TreePop();
    //            treeNodeOpen = false;
    //        }
    //        if (!settings.treeNodeLabel.empty()) {
    //            if (ImGui::TreeNode(settings.treeNodeLabel.c_str())) {
    //                currentTreeNodeLabel = settings.treeNodeLabel;
    //                treeNodeOpen = true;
    //            }
    //        } else {
    //            currentTreeNodeLabel.clear();
    //        }
    //    }

    //    if (treeNodeOpen || settings.treeNodeLabel.empty()) {
    //        DrawWidget(key, value, settings); // 各変数の描画
    //    }
    //}

    //if (treeNodeOpen) {
    //    ImGui::TreePop();
    //}
}

// 描画処理を分ける関数
void GlobalParameter::DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings) {
    std::visit([&](auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int32_t>) {
            if (drawSettings.widgetType == WidgetType::SliderInt) {
                ImGui::SliderInt(itemName.c_str(), &value, static_cast<int>(drawSettings.minValue), static_cast<int>(drawSettings.maxValue));
            }
        }
        else if constexpr (std::is_same_v<T, float>) {
            if (drawSettings.widgetType == WidgetType::DragFloat) {
                ImGui::DragFloat(itemName.c_str(), &value, 0.1f);
            }
        }
        else if constexpr (std::is_same_v<T, Vector3>) {
            if (drawSettings.widgetType == WidgetType::DragFloat3) {
                ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(&value), 0.1f);
            }
        }
        else if constexpr (std::is_same_v<T, bool>) {
            if (drawSettings.widgetType == WidgetType::Checkbox) {
                ImGui::Checkbox(itemName.c_str(), &value);
            }
        }
        else if constexpr (std::is_same_v<T, Vector4>) {
            if (drawSettings.widgetType == WidgetType::ColorEdit4) {
                ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(&value));
            }
        }
        }, item);
}

///============================================================================
/// 値セット
///=============================================================================
template <typename T>
void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value, WidgetType widgetType) {
    Group& group = datas_[groupName]; // グループ取得または作成

    // 現在のツリーノードがスタックにある場合、そのラベルを取得
    std::string treeNodeLabel = treeNodeStack_.empty() ? "" : treeNodeStack_.top();

    // 既存のキーが存在するか確認
    if (group.find(key) != group.end()) {
        auto& [existingItem, existingSettings] = group[key];

        // 値の型が一致していれば更新
        if (std::holds_alternative<T>(existingItem)) {
            existingItem = value;
        }
        else {
            throw std::runtime_error("Type mismatch for key: " + key);
        }

        // 描画設定の更新 (ツリーノードやウィジェットタイプが変更される可能性がある)
        existingSettings.widgetType = widgetType;
        existingSettings.treeNodeLabel = treeNodeLabel;
    }
    else {
        // 新規の場合、値と設定を追加
        DrawSettings settings;
        settings.widgetType = widgetType;
        settings.treeNodeLabel = treeNodeLabel;
        group[key] = { value, settings };
    }
}


///============================================================================
/// アイテム追加
///=============================================================================
template<typename T>
void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T defaultValue, WidgetType widgetType) {
    Group& group = datas_[groupName];

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
    }
    else {
        // 新規アイテムの追加
        DrawSettings settings;
        settings.widgetType = widgetType;
        if (!treeNodeStack_.empty()) {
            settings.treeNodeLabel = treeNodeStack_.top();
        }
        group[key] = { defaultValue, settings }; // 新規データを追加
    }
}



///============================================================================
/// 値取得
///=============================================================================
template<typename T>
T GlobalParameter::GetValue(const std::string& groupName, const std::string& key) const {
    auto itGroup = datas_.find(groupName);
    assert(itGroup != datas_.end());

    const Group& group = datas_.at(groupName);
    auto itItem = group.find(key);
    assert(itItem != group.end());

    return std::get<T>(itItem->second.first);
}



void GlobalParameter::AddTreeNode(const std::string& nodeName) {
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

void GlobalParameter::SaveFile(const std::string& groupName) {
    auto itGroup = datas_.find(groupName);
    assert(itGroup != datas_.end());

    json root;
    for (auto& [itemName, item] : itGroup->second) {
        if (std::holds_alternative<int32_t>(item.first)) {
            root[groupName][itemName] = std::get<int32_t>(item.first);
        }
        else if (std::holds_alternative<uint32_t>(item.first)) {
            root[groupName][itemName] = std::get<uint32_t>(item.first);
        }
        else if (std::holds_alternative<float>(item.first)) {
            root[groupName][itemName] = std::get<float>(item.first);
        }
        else if (std::holds_alternative<Vector2>(item.first)) {
            Vector2 value = std::get<Vector2>(item.first);
            root[groupName][itemName] = json::array({ value.x, value.y });
        }
        else if (std::holds_alternative<Vector3>(item.first)) {
            Vector3 value = std::get<Vector3>(item.first);
            root[groupName][itemName] = json::array({ value.x, value.y, value.z });
        }
        else if (std::holds_alternative<Vector4>(item.first)) {
            Vector4 value = std::get<Vector4>(item.first);
            root[groupName][itemName] = json::array({ value.x, value.y, value.z, value.w });
        }
        else if (std::holds_alternative<bool>(item.first)) {
            root[groupName][itemName] = std::get<bool>(item.first);
        }
    }

    std::filesystem::path dir(kDirectoryPath);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    std::string filePath = kDirectoryPath + groupName + ".json";
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
    if (!std::filesystem::exists(dir)) return;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        const std::filesystem::path& filePath = entry.path();
        if (filePath.extension() != ".json") continue;
        LoadFile(filePath.stem().string());
    }
}

void GlobalParameter::LoadFile(const std::string& groupName) {
    std::string filePath = kDirectoryPath + groupName + ".json";
    std::ifstream ifs(filePath);
    if (ifs.fail()) {
        std::string message = "Failed to open data file for read.";
        MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
        assert(0);
        return;
    }

    json root;
    ifs >> root;
    ifs.close();

    auto itGroup = root.find(groupName);
    assert(itGroup != root.end());

    for (auto itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
        const std::string& itemName = itItem.key();

        // Integer (int32_t)
        if (itItem->is_number_integer()) {
            int32_t value = itItem->get<int32_t>();
            SetValue(groupName, itemName, value,WidgetType::SliderInt);
        }
        // Unsigned Integer (uint32_t)
        else if (itItem->is_number_integer()) {
            uint32_t value = itItem->get<uint32_t>();
            SetValue(groupName, itemName, value,  WidgetType::SliderInt );
        }
        // Float (float)
        else if (itItem->is_number_float()) {
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value),WidgetType::DragFloat);
        }
        // Vector2 (Array of 2 elements)
        else if (itItem->is_array() && itItem->size() == 2) {
            Vector2 value = { itItem->at(0), itItem->at(1) };
            SetValue(groupName, itemName, value, WidgetType::DragFloat2);
        }
        // Vector3 (Array of 3 elements)
        else if (itItem->is_array() && itItem->size() == 3) {
            Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
            SetValue(groupName, itemName, value,WidgetType::DragFloat3);
        }
        // Vector4 (Array of 4 elements)
        else if (itItem->is_array() && itItem->size() == 4) {
            Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
            SetValue(groupName, itemName, value, WidgetType::DragFloat4);
        }
        // Boolean
        else if (itItem->is_boolean()) {
            bool value = itItem->get<bool>();
            SetValue(groupName, itemName, value,WidgetType::Checkbox);
        }
    }
}

    template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value, WidgetType widgetType);
    template void GlobalParameter::SetValue<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, WidgetType widgetType);
    template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value, WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, WidgetType widgetType);
    template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value, WidgetType widgetType);

    template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value, WidgetType widgetType);
    template void GlobalParameter::AddItem<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, WidgetType widgetType);
    template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value, WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, WidgetType widgetType);
    template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value, WidgetType widgetType);

    template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
    template uint32_t GlobalParameter::GetValue<uint32_t>(const std::string& groupName, const std::string& key) const;
    template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
    template Vector2 GlobalParameter::GetValue<Vector2>(const std::string& groupName, const std::string& key) const;
    template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
    template Vector4 GlobalParameter::GetValue<Vector4>(const std::string& groupName, const std::string& key) const;
    template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;

  