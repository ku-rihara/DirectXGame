#pragma once
#include "EasingParameterData.h"
#include <json.hpp>
#include <string>
#include <unordered_map>

/// <summary>
/// イージングクリエータクラス
/// </summary>
template <typename T>
class EasingCreator {
public:
    EasingCreator()  = default;
    ~EasingCreator() = default;

    /// <summary>
    /// パラメータ読み込み
    /// </summary>
    /// <param name="path">ファイルパス</param>
    void LoadParameter(const std::string& path);

    /// <summary>
    /// パラメータ保存
    /// </summary>
    /// <param name="path">ファイルパス</param>
    void SaveParameter(const std::string& path) const;

    /// <summary>
    /// プリセット追加
    /// </summary>
    /// <param name="name">プリセット名</param>
    /// <param name="param">パラメータ</param>
    void AddPreset(const std::string& name, const EasingParameter<T>& param);

    /// <summary>
    /// プリセット削除
    /// </summary>
    /// <param name="name">プリセット名</param>
    void RemovePreset(const std::string& name);

    /// <summary>
    /// プリセット名変更
    /// </summary>
    /// <param name="oldName">旧名</param>
    /// <param name="newName">新名</param>
    void RenamePreset(const std::string& oldName, const std::string& newName);

    /// <summary>
    /// プリセット編集
    /// </summary>
    /// <param name="name">プリセット名</param>
    /// <param name="newParam">新しいパラメータ</param>
    void EditPreset(const std::string& name, const EasingParameter<T>& newParam);

    /// <summary>
    /// JSON変換
    /// </summary>
    /// <param name="j">JSONオブジェクト</param>
    /// <param name="name">名前</param>
    /// <param name="param">パラメータ</param>
    void ToJson(nlohmann::json& j, const std::string& name, const EasingParameter<T>& param) const;

    /// <summary>
    /// JSONから変換
    /// </summary>
    /// <param name="j">JSONオブジェクト</param>
    void FromJson(const nlohmann::json& j);

    /// <summary>
    /// 編集中のパラメータ取得
    /// </summary>
    /// <param name="name">プリセット名</param>
    const EasingParameter<T>* GetEditingParam(const std::string& name) const;

    const std::unordered_map<std::string, EasingParameter<T>>& GetPresets() const { return presets_; }

    void Clear(); //< クリア
    void Edit(); //< 編集
    void SaveSelectedParameter(); //< 選択中のパラメータ保存
    void LoadSelectedParameter(); //< 選択中のパラメータ読み込み
    void AdaptEditorParam(); //< エディタパラメータ適用

private:
    std::unordered_map<std::string, EasingParameter<T>> presets_;
    std::string selectedName_;
    std::string newPresetName_;
    EasingParameter<T> editingParam_;
    char renameBuf_[128]{};
    const std::string kDirectoryPath_ = "Resources/EasingParameter/";

public:
    void SetAllPresets(const std::unordered_map<std::string, EasingParameter<T>>& newPresets);
    const std::unordered_map<std::string, EasingParameter<T>>& GetAllPresets() const;
};