#pragma once
#include "EasingParameterData.h"
#include <json.hpp>
#include <string>
#include <unordered_map>

template <typename T>
class EasingCreator {
public:
    void LoadParameter(const std::string& path);
    void SaveParameter(const std::string& path) const;

    void AddPreset(const std::string& name, const EasingParameter<T>& param);
    void RemovePreset(const std::string& name);
    void RenamePreset(const std::string& oldName, const std::string& newName);
    void EditPreset(const std::string& name, const EasingParameter<T>& newParam);

    const std::unordered_map<std::string, EasingParameter<T>>& GetPresets() const { return presets_; }

    void ToJson(nlohmann::json& j, const std::string& name, const EasingParameter<T>& param) const;
    void FromJson(const nlohmann::json& j);

    const EasingParameter<T>* GetEditingParam(const std::string& name) const;

    void Clear();
    void Edit();

    void SaveSelectedParameter() ;
    void LoadSelectedParameter() ;
    void AdaptEditorParam();

private:
    std::unordered_map<std::string, EasingParameter<T>> presets_;

    // UI状態管理用
    std::string selectedName_;
    std::string newPresetName_;
    EasingParameter<T> editingParam_;
    char renameBuf_[128]{};

    const std ::string kDirectoryPath_ = "Resources/EasingParameter/";

public:
    void SetAllPresets(const std::unordered_map<std::string, EasingParameter<T>>& newPresets);
    const std::unordered_map<std::string, EasingParameter<T>>& GetAllPresets() const;
};
