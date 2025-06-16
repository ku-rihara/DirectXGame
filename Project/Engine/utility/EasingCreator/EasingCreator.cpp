#include "EasingCreator.h"
#include "vector2.h"
#include "vector3.h"
#include <fstream>
#include <type_traits>
#include <imgui.h>

template <typename T>
void EasingCreator<T>::LoadFromFile(const std::string& path) {
    std::ifstream ifs(path);
    nlohmann::json j;
    ifs >> j;
    FromJson(j);
}

template <typename T>
void EasingCreator<T>::SaveToFile(const std::string& path) const {
    std::ofstream ofs(path);
    nlohmann::json j;
    ToJson(j);
    ofs << j.dump(4);
}

template <typename T>
void EasingCreator<T>::AddPreset(const std::string& name, const EasingParameter<T>& param) {
    presets_[name] = param;
}

template <typename T>
void EasingCreator<T>::RemovePreset(const std::string& name) {
    presets_.erase(name);
}

template <typename T>
void EasingCreator<T>::RenamePreset(const std::string& oldName, const std::string& newName) {
    if (presets_.count(oldName)) {
        presets_[newName] = presets_[oldName];
        presets_.erase(oldName);
    }
}

template <typename T>
void EasingCreator<T>::Clear() {
    presets_.clear();
}

template <typename T>
void EasingCreator<T>::EditPreset(const std::string& name, const EasingParameter<T>& newParam) {
    if (presets_.count(name)) {
        presets_[name] = newParam;
    }
}

template <typename T>
void EasingCreator<T>::ToJson(nlohmann::json& j) const {
    for (const auto& [name, param] : presets_) {
        auto& jsonParam = j[name];

        if constexpr (std::is_same_v<T, Vector2>) {
            jsonParam["startValue"] = {param.startValue.x, param.startValue.y};
            jsonParam["endValue"]   = {param.endValue.x, param.endValue.y};
        } else if constexpr (std::is_same_v<T, Vector3>) {
            jsonParam["startValue"] = {param.startValue.x, param.startValue.y, param.startValue.z};
            jsonParam["endValue"]   = {param.endValue.x, param.endValue.y, param.endValue.z};
        } else {
            jsonParam["startValue"] = param.startValue;
            jsonParam["endValue"]   = param.endValue;
        }

        jsonParam["type"]       = static_cast<int>(param.type);
        jsonParam["finishType"] = static_cast<int>(param.finishType);
        jsonParam["maxTime"]    = param.maxTime;
        jsonParam["amplitude"]  = param.amplitude;
        jsonParam["period"]     = param.period;
        jsonParam["backRatio"]  = param.backRatio;
    }
}

template <typename T>
void EasingCreator<T>::FromJson(const nlohmann::json& j) {
    presets_.clear();
    for (auto it = j.begin(); it != j.end(); ++it) {
        EasingParameter<T> param;
        param.type       = static_cast<EasingType>(it.value().at("type").get<int>());
        param.finishType = static_cast<EasingFinishValueType>(it.value().at("finishType").get<int>());
        param.maxTime    = it.value().at("maxTime").get<float>();
        param.amplitude  = it.value().value("amplitude", 0.0f);
        param.period     = it.value().value("period", 0.0f);
        param.backRatio  = it.value().value("backRatio", 0.0f);

        if constexpr (std::is_same_v<T, Vector2>) {
            auto sv          = it.value().at("startValue");
            auto ev          = it.value().at("endValue");
            param.startValue = Vector2{sv[0], sv[1]};
            param.endValue   = Vector2{ev[0], ev[1]};
        } else if constexpr (std::is_same_v<T, Vector3>) {
            auto sv          = it.value().at("startValue");
            auto ev          = it.value().at("endValue");
            param.startValue = Vector3{sv[0], sv[1], sv[2]};
            param.endValue   = Vector3{ev[0], ev[1], ev[2]};
        } else {
            param.startValue = it.value().at("startValue").get<T>();
            param.endValue   = it.value().at("endValue").get<T>();
        }

        presets_[it.key()] = param;
    }
}

template <typename T>
void EasingCreator<T>::Edit() {
    ImGui::Text("Easing Presets");
    ImGui::Separator();

    // ▼ プリセット一覧
    if (ImGui::BeginListBox("Preset List")) {
        for (const auto& [name, param] : presets_) {
            const bool isSelected = (selectedName_ == name);
            if (ImGui::Selectable(name.c_str(), isSelected)) {
                selectedName_ = name;
                editingParam_ = param;
              strncpy_s(renameBuf_, name.c_str(), sizeof(renameBuf_));
                renameBuf_[sizeof(renameBuf_) - 1] = '\0';
            }
        }
        ImGui::EndListBox();
    }

    // ▼ 新規プリセット作成
    char newBuf[128];
    strncpy_s(newBuf, newPresetName_.c_str(), sizeof(newBuf));
    newBuf[sizeof(newBuf) - 1] = '\0';
    if (ImGui::InputText("New Preset Name", newBuf, sizeof(newBuf))) {
        newPresetName_ = newBuf;
    }

    if (ImGui::Button("Add New Preset") && !newPresetName_.empty()) {
        EasingParameter<T> defaultParam;
        AddPreset(newPresetName_, defaultParam);
        selectedName_ = newPresetName_;
        editingParam_ = defaultParam;
        strncpy_s(renameBuf_, newPresetName_.c_str(), sizeof(renameBuf_));
        renameBuf_[sizeof(renameBuf_) - 1] = '\0';
        newPresetName_.clear();
    }

    ImGui::Separator();

    // ▼ 編集 UI
    if (!selectedName_.empty() && presets_.count(selectedName_)) {
        ImGui::Text("Edit Preset: %s", selectedName_.c_str());

        if (ImGui::InputText("Rename", renameBuf_, sizeof(renameBuf_))) {}
        if (std::string(renameBuf_) != selectedName_ && ImGui::Button("Apply Rename")) {
            RenamePreset(selectedName_, renameBuf_);
            selectedName_ = renameBuf_;
        }

        ImGui::DragFloat("Max Time", &editingParam_.maxTime, 0.01f);
        ImGui::DragFloat("Amplitude", &editingParam_.amplitude, 0.01f);
        ImGui::DragFloat("Period", &editingParam_.period, 0.01f);
        ImGui::DragFloat("Back Ratio", &editingParam_.backRatio, 0.01f);

        if constexpr (std::is_same_v<T, float>) {
            ImGui::DragFloat("Start Value", &editingParam_.startValue,0.01f);
            ImGui::DragFloat("End Value", &editingParam_.endValue, 0.01f);
        } else if constexpr (std::is_same_v<T, Vector2>) {
            ImGui::DragFloat2("Start Value", &editingParam_.startValue.x, 0.01f);
            ImGui::DragFloat2("End Value", &editingParam_.endValue.x, 0.01f);
        } else if constexpr (std::is_same_v<T, Vector3>) {
            ImGui::DragFloat3("Start Value", &editingParam_.startValue.x, 0.01f);
            ImGui::DragFloat3("End Value", &editingParam_.endValue.x, 0.01f);
        }

   int easingType = static_cast<int>(editingParam_.type);
        if (ImGui::Combo("Easing Type", &easingType, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
            editingParam_.type = static_cast<EasingType>(easingType);
        }

        int finishType = static_cast<int>(editingParam_.finishType);
        if (ImGui::Combo("Finish Type", &finishType, FinishTypeLabels, static_cast<int>(EasingFinishValueType::COUNT))) {
            editingParam_.finishType = static_cast<EasingFinishValueType>(finishType);
        }

        if (ImGui::Button("Apply Edit")) {
            EditPreset(selectedName_, editingParam_);
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete Preset")) {
            RemovePreset(selectedName_);
            selectedName_.clear();
        }
    }

    ImGui::Separator();

    if (ImGui::Button("Clear All Presets")) {
        Clear();
        selectedName_.clear();
    }
}

template <typename T>
void EasingCreator<T>::SetAllPresets(const std::unordered_map<std::string, EasingParameter<T>>& newPresets) {
    presets_ = newPresets;
}

template <typename T>
const std::unordered_map<std::string, EasingParameter<T>>& EasingCreator<T>::GetAllPresets() const {
    return presets_;
}

// 明示的インスタンス化
template class EasingCreator<float>;
template class EasingCreator<Vector2>;
template class EasingCreator<Vector3>;
