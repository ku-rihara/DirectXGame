#include"EasingCreator.h"
#include "vector2.h"
#include "vector3.h"
#include <fstream>

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
    ofs << ToJson().dump(4);
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
void EasingCreator<T>::EditPreset(const std::string& name, const EasingParameter<T>& newParam) {
    if (presets_.count(name)) {
        presets_[name] = newParam;
    }
}

template <typename T>
void EasingCreator<T>::ToJson(nlohmann::json& j)  {
    for (const auto& [name, param] : params_) {
        if constexpr (std::is_same_v<T, Vector2>) {
            j[name]["startValue"] = {param.startValue.x, param.startValue.y};
            j[name]["endValue"]   = {param.endValue.x, param.endValue.y};
        } else if constexpr (std::is_same_v<T, Vector3>) {
            j[name]["startValue"] = {param.startValue.x, param.startValue.y, param.startValue.z};
            j[name]["endValue"]   = {param.endValue.x, param.endValue.y, param.endValue.z};
        } else {
            j[name]["startValue"] = param.startValue;
            j[name]["endValue"]   = param.endValue;
        }

        j[name]["time"]      = param.time;
        j[name]["maxTime"]   = param.maxTime;
        j[name]["backRatio"] = param.backRatio;
        j[name]["amplitude"] = param.amplitude;
        j[name]["period"]    = param.period;
    }
}

template <typename T>
void EasingCreator<T>::FromJson(const nlohmann::json& j) {
    presets_.clear();
    for (auto it = j.begin(); it != j.end(); ++it) {
        EasingParameter<T> param;
        param.type         = static_cast<EasingType>(it.value().at("type").get<int>());
        param.finishType   = static_cast<EasingFinishValueType>(it.value().at("finishType").get<int>());
        param.startValue   = it.value().at("startValue").get<T>();
        param.endValue     = it.value().at("endValue").get<T>();
        param.maxTime      = it.value().at("maxTime").get<float>();
        param.amplitude    = it.value().value("amplitude", 0.0f);
        param.period       = it.value().value("period", 0.0f);
        param.backRatio    = it.value().value("backRatio", 0.0f);
        presets_[it.key()] = param;
    }
}

template class EasingCreator<float>;
template class EasingCreator<Vector2>;
template class EasingCreator<Vector3>;