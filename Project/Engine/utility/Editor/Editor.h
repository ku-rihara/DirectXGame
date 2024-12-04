#pragma once

#include <string>
#include <json.hpp>
#include <functional>

/// <summary>
/// パラメータ編集・保存・読み込みを管理するクラス
/// </summary>
class Editor {
public:
    using json = nlohmann::json;

    /// ImGuiウィジェットを通じてパラメータを編集
    template<typename T>
    void EditParameter(const std::string& name, T& parameter);

    /// JSONデータをファイルに保存
    template<typename T>
    void SaveToFile(const std::string& filepath, const T& data) const;

    /// JSONデータをファイルから読み込み
    template<typename T>
    void LoadFromFile(const std::string& filepath, T& data) const;

    /// データの保存ロジック
    void Save(const std::string& filepath, const std::function<json()>& serialize) const;

    /// データの読み込みロジック
    void Load(const std::string& filepath, const std::function<void(const json&)>& deserialize) const;

    // Serialize
    template<typename T>
    static json Serialize(const T& obj);

    template<typename T>
    static void Deserialize(T& obj, const json& data);
};
