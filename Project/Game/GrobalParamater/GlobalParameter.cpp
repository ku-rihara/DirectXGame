
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
    // デバッグ用ウィンドウを作成
    if (!ImGui::Begin("Global Parameter", nullptr, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    // メニューバーの作成
    if (!ImGui::BeginMenuBar())
        return;

    // 各グループの処理
    for (std::map<std::string, Group>::iterator groupIt = datas_.begin(); groupIt != datas_.end(); ++groupIt) {
        const std::string& groupName = groupIt->first;
        Group& group = groupIt->second;

        if (!ImGui::BeginMenu(groupName.c_str()))
            continue;

        // 各アイテムの処理
        for (std::map<std::string, Parameter>::iterator itemIt = group.begin(); itemIt != group.end(); ++itemIt) {
            const std::string& itemName = itemIt->first;
            Parameter& param = itemIt->second;
            Item& item = param.first;  // 値
            const DrawSettings& drawSettings = param.second;  // 描画設定

            // ツリーラベルが設定されている場合、TreeNodeを開始する
            if (!drawSettings.treeNodeLabel.empty()) {
                if (ImGui::TreeNode(drawSettings.treeNodeLabel.c_str())) {
                    // ツリー内にアイテムを描画
                    DrawWidget(itemName, item, drawSettings);
                    ImGui::TreePop();  // ツリーの終了
                }
            }
            else {
                // ツリーラベルがない場合、通常のウィジェットを描画
                DrawWidget(itemName, item, drawSettings);
            }
        }

        // セーブボタン
        ImGui::Text("\n");
        if (ImGui::Button("Save")) {
            SaveFile(groupName);
            std::string message = std::format("{}.json saved.", groupName);
            MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
    ImGui::End();
#endif // _DEBUG
}

// 描画処理を分ける関数
void GlobalParameter::DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings) {
    switch (drawSettings.widgetType) {
    case DrawSettings::WidgetType::SliderInt:
        if (std::holds_alternative<int32_t>(item)) {
            int32_t* ptr = std::get_if<int32_t>(&item);
            ImGui::SliderInt(itemName.c_str(), ptr, static_cast<int>(drawSettings.minValue), static_cast<int>(drawSettings.maxValue));
        }
        break;

    case DrawSettings::WidgetType::DragFloat:
        if (std::holds_alternative<float>(item)) {
            float* ptr = std::get_if<float>(&item);
            ImGui::DragFloat(itemName.c_str(), ptr, 0.1f, drawSettings.minValue, drawSettings.maxValue);
        }
        break;

    case DrawSettings::WidgetType::DragFloat2:
        if (std::holds_alternative<Vector2>(item)) {
            Vector2* ptr = std::get_if<Vector2>(&item);
            ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, drawSettings.minValue, drawSettings.maxValue);
        }
        break;

    case DrawSettings::WidgetType::DragFloat3:
        if (std::holds_alternative<Vector3>(item)) {
            Vector3* ptr = std::get_if<Vector3>(&item);
            ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, drawSettings.minValue, drawSettings.maxValue);
        }
        break;

    case DrawSettings::WidgetType::DragFloat4:
        if (std::holds_alternative<Vector4>(item)) {
            Vector4* ptr = std::get_if<Vector4>(&item);
            ImGui::DragFloat4(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f, drawSettings.minValue, drawSettings.maxValue);
        }
        break;

    case DrawSettings::WidgetType::Checkbox:
        if (std::holds_alternative<bool>(item)) {
            bool* ptr = std::get_if<bool>(&item);
            ImGui::Checkbox(itemName.c_str(), ptr);
        }
        break;

    case DrawSettings::WidgetType::ColorEdit4:
        if (std::holds_alternative<Vector4>(item)) {
            Vector4* ptr = std::get_if<Vector4>(&item);
            ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(ptr));
        }
        break;

    default:
        break;
    }
}


///============================================================================
/// 値セット
///=============================================================================
template<typename T>
void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value, GlobalParameter::DrawSettings::WidgetType widgetType) {
    Group& group = datas_[groupName];

    // ツリーが開始されている場合、ツリー内にアイテムを追加
    bool isInTreeNode = !treeNodeStack_.empty();

    // ツリー内である場合、ツリーラベルを使用する
    std::string treeNodeLabel;
    if (isInTreeNode) {
        if (!treeNodeStack_.empty()) {
            treeNodeLabel = treeNodeStack_.top(); 
        }
    }

    DrawSettings settings;
    settings.widgetType = widgetType;  // WidgetTypeを設定
    settings.treeNodeLabel = treeNodeLabel;  // ツリーラベルを設定

    // 値とウィジェット設定をセット
    group[key] = { value, settings };
}

///============================================================================
/// アイテム追加
///=============================================================================
template<typename T>
void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T value, GlobalParameter::DrawSettings::WidgetType widgetType) {
    Group& group = datas_[groupName];
    if (group.find(key) != group.end()) {
        SetValue(groupName, key, value, { widgetType });  // 既にあれば更新
    }
    else {
        DrawSettings settings;
        settings.widgetType = widgetType;
        group[key] = { value, settings };  // 新規追加
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



// ツリーのノード追加
void GlobalParameter::AddTreeNode(const std::string& nodeName) {
    treeNodeStack_.push(nodeName);  // 現在のツリー状態をスタックに追加
}

// ツリーのノードを閉じる
void GlobalParameter::AddTreePoP() {
    if (!treeNodeStack_.empty()) {
        treeNodeStack_.pop();  // スタックから最後のノードを削除
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
            SetValue(groupName, itemName, value,DrawSettings::WidgetType::SliderInt);
        }
        // Unsigned Integer (uint32_t)
        else if (itItem->is_number_integer()) {
            uint32_t value = itItem->get<uint32_t>();
            SetValue(groupName, itemName, value,  DrawSettings::WidgetType::SliderInt );
        }
        // Float (float)
        else if (itItem->is_number_float()) {
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value),DrawSettings::WidgetType::DragFloat);
        }
        // Vector2 (Array of 2 elements)
        else if (itItem->is_array() && itItem->size() == 2) {
            Vector2 value = { itItem->at(0), itItem->at(1) };
            SetValue(groupName, itemName, value, DrawSettings::WidgetType::DragFloat2);
        }
        // Vector3 (Array of 3 elements)
        else if (itItem->is_array() && itItem->size() == 3) {
            Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
            SetValue(groupName, itemName, value,DrawSettings::WidgetType::DragFloat3);
        }
        // Vector4 (Array of 4 elements)
        else if (itItem->is_array() && itItem->size() == 4) {
            Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
            SetValue(groupName, itemName, value, DrawSettings::WidgetType::DragFloat4);
        }
        // Boolean
        else if (itItem->is_boolean()) {
            bool value = itItem->get<bool>();
            SetValue(groupName, itemName, value,DrawSettings::WidgetType::Checkbox);
        }
    }
}

    template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value, GlobalParameter::DrawSettings::WidgetType widgetType);

    template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector2>(const std::string& groupName, const std::string& key, Vector2 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<Vector4>(const std::string& groupName, const std::string& key, Vector4 value, GlobalParameter::DrawSettings::WidgetType widgetType);
    template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value, GlobalParameter::DrawSettings::WidgetType widgetType);

    template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
    template uint32_t GlobalParameter::GetValue<uint32_t>(const std::string& groupName, const std::string& key) const;
    template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
    template Vector2 GlobalParameter::GetValue<Vector2>(const std::string& groupName, const std::string& key) const;
    template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
    template Vector4 GlobalParameter::GetValue<Vector4>(const std::string& groupName, const std::string& key) const;
    template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;

  