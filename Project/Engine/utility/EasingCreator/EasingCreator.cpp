#include "EasingCreator.h"
#include "vector2.h"
#include "vector3.h"
#include"MathFunction.h"
#include <fstream>
#include <imgui.h>
#include <type_traits>

template <typename T>
void EasingCreator<T>::LoadParameter(const std::string& path) {
    presets_.clear();

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        // JSONファイルのみを対象とする
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        // JSONファイルを読み込む
        std::ifstream ifs(entry.path());
        if (!ifs.is_open()) {
            continue;
        }
        nlohmann::json j;
        ifs >> j;
        FromJson(j);
    }
    
    if (!selectedName_.empty()) {
        auto it = presets_.find(selectedName_);
        if (it != presets_.end()) {
            editingParam_ = it->second;
        } else {
            selectedName_.clear();
        }
    }
}

template <typename T>
void EasingCreator<T>::SaveParameter(const std::string& path) const {
    std::filesystem::create_directories(path);

    for (const auto& [name, param] : presets_) {
        nlohmann::json singlePresetJson;
        ToJson(singlePresetJson, name, param);

        std::filesystem::path filepath = std::filesystem::path(path) / (name + ".json");
        std::ofstream ofs(filepath);
        if (ofs.is_open()) {
            ofs << singlePresetJson.dump(4);
        }
    }
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
void EasingCreator<T>::ToJson(nlohmann::json& j, const std::string& name, const EasingParameter<T>& param) const {
    auto& jsonParam = j[name];



    if constexpr (std::is_same_v<T, Vector3>) {
        jsonParam["startValue"] = {param.startValue.x, param.startValue.y, param.startValue.z};
        jsonParam["endValue"]   = {param.endValue.x, param.endValue.y, param.endValue.z};
    } else if constexpr (std::is_same_v<T, Vector2>) {
        jsonParam["startValue"]        = {param.startValue.x, param.startValue.y};
        jsonParam["endValue"]          = {param.endValue.x, param.endValue.y};
        jsonParam["adaptVec2AxisType"] = static_cast<int>(param.adaptVec2AxisType);
    } else if constexpr (std::is_same_v<T, float>) {

        jsonParam["startValue"]         = param.startValue;
        jsonParam["endValue"]           = param.endValue;
        jsonParam["adaptFloatAxisType"] = static_cast<int>(param.adaptFloatAxisType);
    }

    jsonParam["type"]       = static_cast<int>(param.type);
    jsonParam["finishType"] = static_cast<int>(param.finishType);
    jsonParam["maxTime"]    = param.maxTime;
    jsonParam["amplitude"]  = param.amplitude;
    jsonParam["period"]     = param.period;
    jsonParam["backRatio"]  = param.backRatio;
}

template <typename T>
void EasingCreator<T>::FromJson(const nlohmann::json& j) {
    for (auto inner = j.begin(); inner != j.end(); ++inner) {
        const auto& val = inner.value();

        if (!val.contains("type") || !val.contains("startValue") || !val.contains("endValue"))
            continue;

        EasingParameter<T> param;
        param.type       = static_cast<EasingType>(val["type"].get<int>());
        param.finishType = static_cast<EasingFinishValueType>(val["finishType"].get<int>());
        param.maxTime    = val["maxTime"].get<float>();
        param.amplitude  = val.value("amplitude", 0.0f);
        param.period     = val.value("period", 0.0f);
        param.backRatio  = val.value("backRatio", 0.0f);

        if constexpr (std::is_same_v<T, Vector3>) {
            auto sv          = val["startValue"];
            auto ev          = val["endValue"];
            param.startValue = Vector3{sv[0], sv[1], sv[2]};
            param.endValue   = Vector3{ev[0], ev[1], ev[2]};
        } else if constexpr (std::is_same_v<T, Vector2>) {
            auto sv                 = val["startValue"];
            auto ev                 = val["endValue"];
            param.startValue        = Vector2{sv[0], sv[1]};
            param.endValue          = Vector2{ev[0], ev[1]};
            param.adaptVec2AxisType = static_cast<AdaptVector2AxisType>(val.value("adaptVec2AxisType", 0));
        } else if constexpr (std::is_same_v<T, float>) {
            param.startValue         = val["startValue"].get<T>();
            param.endValue           = val["endValue"].get<T>();
            param.adaptFloatAxisType = static_cast<AdaptFloatAxisType>(val.value("adaptFloatAxisType", 0));
        }

        presets_[inner.key()] = param;
      
    }
}

template <typename T>
void EasingCreator<T>::Edit() {
    ImGui::Text("Easing Presets");
    ImGui::Separator();

    //  プリセット一覧
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

    //  新規プリセット作成
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

    //  編集 UI
    if (!selectedName_.empty() && presets_.count(selectedName_)) {
        ImGui::Text("Edit Preset: %s", selectedName_.c_str());

       
        // イージングパラメータ
        ImGui::DragFloat("Max Time", &editingParam_.maxTime, 0.01f);
        ImGui::DragFloat("Amplitude", &editingParam_.amplitude, 0.01f);
        ImGui::DragFloat("Period", &editingParam_.period, 0.01f);
        ImGui::DragFloat("Back Ratio", &editingParam_.backRatio, 0.01f);

        int adaptFloatAxisType = static_cast<int>(editingParam_.adaptFloatAxisType);
        int adaptVec2AxisType  = static_cast<int>(editingParam_.adaptVec2AxisType);

        // スタート、終了位置の入力
        if constexpr (std::is_same_v<T, float>) {
            ImGui::DragFloat("Start Value", &editingParam_.startValue, 0.01f);
            ImGui::DragFloat("End Value", &editingParam_.endValue, 0.01f);
            if (ImGui::Button("Change Radian")) {
                editingParam_.startValue = toRadian(editingParam_.startValue);
                editingParam_.endValue   = toRadian(editingParam_.endValue);
            }

            if (ImGui::Combo("AdaptAxis Type", &adaptFloatAxisType, AdaptFloatAxisTypeLabels.data(), static_cast<int>(AdaptFloatAxisTypeLabels.size()))) {
                editingParam_.adaptFloatAxisType = static_cast<AdaptFloatAxisType>(adaptFloatAxisType);
            }

        } else if constexpr (std::is_same_v<T, Vector2>) { // vec2
            ImGui::DragFloat2("Start Value", &editingParam_.startValue.x, 0.01f);
            ImGui::DragFloat2("End Value", &editingParam_.endValue.x, 0.01f);

            // 軸のタイプの選択(Vector2)
            if (ImGui::Combo("AdaptAxis Type", &adaptVec2AxisType, AdaptVector2AxisTypeLabels.data(), static_cast<int>(AdaptVector2AxisTypeLabels.size()))) {
                editingParam_.adaptVec2AxisType = static_cast<AdaptVector2AxisType>(adaptVec2AxisType);
            }

        } else if constexpr (std::is_same_v<T, Vector3>) { // vec3
            ImGui::DragFloat3("Start Value", &editingParam_.startValue.x, 0.01f);
            ImGui::DragFloat3("End Value", &editingParam_.endValue.x, 0.01f);
        }

        // イージングタイプの設定
        int easingType = static_cast<int>(editingParam_.type);
        if (ImGui::Combo("Easing Type", &easingType, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
            editingParam_.type = static_cast<EasingType>(easingType);
        }

        // 終了時の値を設定
        int finishType = static_cast<int>(editingParam_.finishType);
        if (ImGui::Combo("Finish Type", &finishType, FinishTypeLabels.data(), static_cast<int>(FinishTypeLabels.size()))) {
            editingParam_.finishType = static_cast<EasingFinishValueType>(finishType);
        }

        EditPreset(selectedName_, editingParam_);

        // 削除ボタン
        ImGui::SameLine();
        if (ImGui::Button("Delete Preset")) {
            RemovePreset(selectedName_);
            selectedName_.clear();
        }
    }

    ImGui::Separator();
}

template <typename T>
const EasingParameter<T>* EasingCreator<T>::GetEditingParam(const std::string& name) const {
    std::string trimmedName = name;

    const std::string_view ext = ".json";
    if (trimmedName.size() >= ext.size() && trimmedName.compare(trimmedName.size() - ext.size(), ext.size(), ext) == 0) {
        trimmedName.erase(trimmedName.size() - ext.size());
    }

    auto it = presets_.find(trimmedName);
    if (it != presets_.end()) {
        return &(it->second);
    }
    return nullptr;
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
