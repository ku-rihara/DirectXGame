
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
    for (auto& [groupName, group] : datas_) {
        if (!ImGui::BeginMenu(groupName.c_str()))
            continue;

        // 各アイテムの処理
        for (auto& [itemName, item] : group) {
            if (std::holds_alternative<int32_t>(item)) {
                int32_t* ptr = std::get_if<int32_t>(&item);
                ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
            }
            else if (std::holds_alternative<uint32_t>(item)) {
                uint32_t* ptr = std::get_if<uint32_t>(&item);

                // 一時的に int 型として操作する
                int tempValue = static_cast<int>(*ptr);

                // ImGui に tempValue を渡して操作
                if (ImGui::SliderInt(itemName.c_str(), &tempValue, 0, 100)) {
                    // 値が変更された場合に元の uint32_t に戻す
                    *ptr = static_cast<uint32_t>(tempValue);
                }
            }
            else if (std::holds_alternative<float>(item)) {
                float* ptr = std::get_if<float>(&item);
                ImGui::DragFloat(itemName.c_str(), ptr, 0.1f);
            }
            else if (std::holds_alternative<Vector2>(item)) {
                Vector2* ptr = std::get_if<Vector2>(&item);
                ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
            }
            else if (std::holds_alternative<Vector3>(item)) {
                Vector3* ptr = std::get_if<Vector3>(&item);
                ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
            }
            else if (std::holds_alternative<Vector4>(item)) {
                Vector4* ptr = std::get_if<Vector4>(&item);
                ImGui::ColorEdit4(itemName.c_str(), reinterpret_cast<float*>(ptr));
            }
            else if (std::holds_alternative<bool>(item)) {
                bool* ptr = std::get_if<bool>(&item);
                ImGui::Checkbox(itemName.c_str(), ptr);
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

/// =====================================================================================
/// 指定グループとキーに対して値を設定
/// 任意の型に対応
/// =====================================================================================
template<typename T> void GlobalParameter::SetValue(const std::string& groupName, const std::string& key, T value) {
    Group& group = datas_[groupName];
    Item newItem{};
    newItem = value;
    group[key] = value;
}

/// =====================================================================================
/// 新しいアイテムをグループに追加
/// すでに存在する場合は`SetValue`で更新
/// =====================================================================================
template<typename T> void GlobalParameter::AddItem(const std::string& groupName, const std::string& key, T value) {
    json root;
    json::iterator itGroup = root.find(groupName);
    if (itGroup != root.end()) {
        SetValue(groupName, key, value);
    }
}

/// =====================================================================================
/// グループのデータをファイルに保存
/// =====================================================================================
void GlobalParameter::SaveFile(const std::string& groupName) {

    // グループが存在することを確認
    auto itGroup = datas_.find(groupName);
    assert(itGroup != datas_.end());

    json root = json::object();
    root[groupName] = json::object();

    // 各アイテムをJSON形式に変換
    for (auto& [itemName, item] : itGroup->second) {
        if (std::holds_alternative<int32_t>(item)) {
            root[groupName][itemName] = std::get<int32_t>(item);
        }
        else if (std::holds_alternative<uint32_t>(item)) {
            root[groupName][itemName] = std::get<uint32_t>(item);
        }
        else if (std::holds_alternative<float>(item)) {
            root[groupName][itemName] = std::get<float>(item);
        }
        else if (std::holds_alternative<Vector2>(item)) {
            Vector2 value = std::get<Vector2>(item);
            root[groupName][itemName] = json::array({ value.x, value.y });
        }
        else if (std::holds_alternative<Vector3>(item)) {
            Vector3 value = std::get<Vector3>(item);
            root[groupName][itemName] = json::array({ value.x, value.y, value.z });
        }
        else if (std::holds_alternative<Vector4>(item)) {
            Vector4 value = std::get<Vector4>(item);
            root[groupName][itemName] = json::array({ value.x, value.y, value.z, value.w });
        }
        else if (std::holds_alternative<bool>(item)) {
            root[groupName][itemName] = std::get<bool>(item);
        }
    }

    // ディレクトリの作成
    std::filesystem::path dri(kDirectoryPath);
    if (!std::filesystem::exists(dri)) {
        std::filesystem::create_directories(dri);
    }

    // ファイル書き込み
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
    std::filesystem::path driPath(kDirectoryPath);
    if (!std::filesystem::exists(driPath)) {
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(driPath)) {
        const std::filesystem::path& filePath = entry.path();
        std::string extension = filePath.extension().string();
        if (extension.compare(".json") != 0) {
            continue;
        }
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
        if (itItem->is_number_integer()) {
            int32_t value = itItem->get<int32_t>();
            SetValue(groupName, itemName, value);
        }
        else if (itItem->is_number_integer()) {
            uint32_t value = itItem->get<uint32_t>();
            SetValue(groupName, itemName, static_cast<uint32_t>(value));
        }
        else if (itItem->is_number_float()) {
            double value = itItem->get<double>();
            SetValue(groupName, itemName, static_cast<float>(value));
        }
        else if (itItem->is_array() && itItem->size() == 2) {
            Vector2 value = { itItem->at(0), itItem->at(1) };
            SetValue(groupName, itemName, value);
        }
        else if (itItem->is_array() && itItem->size() == 3) {
            Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
            SetValue(groupName, itemName, value);
        }
        else if (itItem->is_array() && itItem->size() == 4) {
            Vector4 value = { itItem->at(0), itItem->at(1), itItem->at(2), itItem->at(3) };
            SetValue(groupName, itemName, value);
        }
        else if (itItem->is_boolean()) {
            bool value = itItem->get<bool>();
            SetValue(groupName, itemName, value);
        }
    }
}

template<typename T> T GlobalParameter::GetValue(const std::string& groupName, const std::string& key) const {
    auto itGroup = datas_.find(groupName);
    assert(itGroup != datas_.end());

    const Group& group = datas_.at(groupName);
    auto itItem = group.find(key);
    assert(itItem != group.end());

    return std::get<T>(itItem->second);
}


    template void GlobalParameter::SetValue<int>(const std::string& groupName, const std::string& key, int value);
    template void GlobalParameter::SetValue<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value);
    template void GlobalParameter::SetValue<float>(const std::string& groupName, const std::string& key, float value);
    template void GlobalParameter::SetValue<Vector2>(const std::string& groupName, const std::string& key, Vector2 value);
    template void GlobalParameter::SetValue<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
    template void GlobalParameter::SetValue<Vector4>(const std::string& groupName, const std::string& key, Vector4 value);
    template void GlobalParameter::SetValue<bool>(const std::string& groupName, const std::string& key, bool value);

    template void GlobalParameter::AddItem<int>(const std::string& groupName, const std::string& key, int value);
    template void GlobalParameter::AddItem<uint32_t>(const std::string& groupName, const std::string& key, uint32_t value);
    template void GlobalParameter::AddItem<float>(const std::string& groupName, const std::string& key, float value);
    template void GlobalParameter::AddItem<Vector2>(const std::string& groupName, const std::string& key, Vector2 value);
    template void GlobalParameter::AddItem<Vector3>(const std::string& groupName, const std::string& key, Vector3 value);
    template void GlobalParameter::AddItem<Vector4>(const std::string& groupName, const std::string& key, Vector4 value);
    template void GlobalParameter::AddItem<bool>(const std::string& groupName, const std::string& key, bool value);

    template int32_t GlobalParameter::GetValue<int32_t>(const std::string& groupName, const std::string& key) const;
    template uint32_t GlobalParameter::GetValue<uint32_t>(const std::string& groupName, const std::string& key) const;
    template float GlobalParameter::GetValue<float>(const std::string& groupName, const std::string& key) const;
    template Vector2 GlobalParameter::GetValue<Vector2>(const std::string& groupName, const std::string& key) const;
    template Vector3 GlobalParameter::GetValue<Vector3>(const std::string& groupName, const std::string& key) const;
    template Vector4 GlobalParameter::GetValue<Vector4>(const std::string& groupName, const std::string& key) const;
    template bool GlobalParameter::GetValue<bool>(const std::string& groupName, const std::string& key) const;

  