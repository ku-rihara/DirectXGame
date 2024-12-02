#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include <map>
#include <string>
#include <variant>
#include <json.hpp>

class GlobalParameter {
private:

    /// 格納できる複数の値の型を定義
    using Item = std::variant
        <int32_t,uint32_t, float, Vector2, Vector3, Vector4, bool>;

    // キーとその値を関連付けるマップ
    using Group = std::map<std::string, Item>;

    /// json
    using json = nlohmann::json;

private:

    ///=================================================================================
    ///private variant
    ///=================================================================================

    // グループ名(キー)とグループのデータ
    std::map<std::string, Group> datas_;

    // 値が変更されたかどうかのフラグ
    std::map<std::string, bool> isValueChanged_;

    // データを保存する際のディレクトリパス
    const std::string kDirectoryPath = "Resources/GlobalParameter/";
public:

    static GlobalParameter* GetInstance();

    ///=================================================================================
   ///public method
   ///=================================================================================

    ///　　更新
    void Update();

    // 新しいグループを作成
    void CreateGroup(const std::string& groupName);

    // 値を設定する
    template<typename T> void SetValue(const std::string& groupName, const std::string& key, T value);

    // 新しいアイテムをグループに追加する
    template<typename T> void AddItem(const std::string& groupName, const std::string& key, T value);

    // 値を取得する
    template<typename T> T GetValue(const std::string& groupName, const std::string& key) const;

    // ------------------------------------------------------------------------------
    // ファイルへの保存・読み込み
    // ------------------------------------------------------------------------------

     // 特定のグループのデータをファイルに保存する
    void SaveFile(const std::string& groupName);

    // すべてのグループのデータをファイルから読み込む
    void LoadFiles();

    // 特定のグループのデータをファイルから読み込む
    void LoadFile(const std::string& groupName);
};