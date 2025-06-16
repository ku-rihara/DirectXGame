#pragma once
#include "EasingParameterData.h"
#include <json.hpp>
#include <string>
#include <unordered_map>

template <typename T>
class EasingCreator {
public:
    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path) const;

    void AddPreset(const std::string& name, const EasingParameter<T>& param);
    void RemovePreset(const std::string& name);
    void RenamePreset(const std::string& oldName, const std::string& newName);
    void EditPreset(const std::string& name, const EasingParameter<T>& newParam);

    const std::unordered_map<std::string, EasingParameter<T>>& GetPresets() const { return presets_; }

    void ToJson(nlohmann::json& j) const;
    void FromJson(const nlohmann::json& j);

    void Clear(); // 新規作成相当

    void Edit();

private:
    const std ::string kDirectoryPath_ = "Resources/EasingParameter";
    std::unordered_map<std::string, EasingParameter<T>> presets_;

     // UI状態管理用
    std::string selectedName_;
    std::string newPresetName_;
    EasingParameter<T> editingParam_;
    char renameBuf_[128]{};

public:
    void SetAllPresets(const std::unordered_map<std::string, EasingParameter<T>>& newPresets);
    const std::unordered_map<std::string, EasingParameter<T>>& GetAllPresets() const;
};
