#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <functional>
#include <json.hpp>
#include <map>
#include <string>
#include <variant>
#include <vector>

/// <summary>
/// グローバルパラメータ管理クラス
/// </summary>
namespace KetaEngine {

class GlobalParameter {
public:
private:
    struct BoundItem {
        std::function<void()> pullVariant;
        std::function<void()> pushVariant;
    };

    using Item  = std::variant<int32_t, uint32_t, float, Vector2, Vector3, Vector4, bool, std::string>;
    using Group = std::map<std::string, Item>;
    using json  = nlohmann::json;

private:
    GlobalParameter()                                  = default;
    ~GlobalParameter()                                 = default;
    // コピー禁止
    GlobalParameter(const GlobalParameter&)            = delete;
    GlobalParameter& operator=(const GlobalParameter&) = delete;

public:
    static GlobalParameter* GetInstance();

    /// <summary>
    /// グループ作成
    /// </summary>
    /// <param name="groupName">グループ名</param>
    void CreateGroup(const std::string& groupName);

    /// <summary>
    /// 値設定
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="value">値</param>
    template <typename T>
    void SetValue(const std::string& groupName, const std::string& key, const T& value);

    /// <summary>
    /// アイテム追加
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="value">値</param>
    template <typename T>
    void AddItem(const std::string& groupName, const std::string& key, const T& value);

    /// <summary>
    /// 値取得
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    template <typename T>
    T GetValue(const std::string& groupName, const std::string& key) const;

    /// <summary>
    /// 変数登録
    /// </summary>
    /// <param name="group">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="variable">変数ポインタ</param>
    template <typename T>
    void Regist(const std::string& group, const std::string& key, T* variable);

    /// <summary>
    /// 配列値設定
    /// </summary>
    template <typename T>
    void SetArrayValue(const std::string& groupName, const std::string& key, const std::vector<T>& value);

    /// <summary>
    /// 配列値取得
    /// </summary>
    template <typename T>
    std::vector<T> GetArrayValue(const std::string& groupName, const std::string& key) const;

    /// <summary>
    /// 可変長リスト変数登録
    /// </summary>
    template <typename T>
    void RegistArray(const std::string& group, const std::string& key, std::vector<T>* variable) {
        auto& groupArrays = arrayDates_[group];
        if (groupArrays.find(key) == groupArrays.end()) {
            SetArrayValue(group, key, *variable);
        }

        BoundItem item;
        item.pullVariant = [this, group, key, variable]() {
            *variable = GetArrayValue<T>(group, key);
        };
        item.pushVariant = [this, group, key, variable]() {
            SetArrayValue(group, key, *variable);
        };

        registerParams_[group].emplace_back(std::move(item));
    }

    /// <summary>
    /// グループコピー
    /// </summary>
    /// <param name="fromGroup">コピー元</param>
    /// <param name="toGroup">コピー先</param>
    void CopyGroup(const std::string& fromGroup, const std::string& toGroup);

    /// <summary>
    /// グループのパラメータ同期
    /// </summary>
    /// <param name="group">グループ名</param>
    void SyncParamForGroup(const std::string& group);

    /// <summary>
    /// グループ存在確認
    /// </summary>
    /// <param name="groupName">グループ名</param>
    bool HasGroup(const std::string& groupName) const;

    /// <summary>
    /// ファイル読み込み
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void LoadFile(const std::string& groupName, const std::string& fileName = "Application");

    /// <summary>
    /// ファイル保存
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void SaveFile(const std::string& groupName, const std::string& fileName = "Application");

    /// <summary>
    /// ImGui用パラメータ保存
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void ParamSaveForImGui(const std::string& groupName, const std::string& fileName = "Application");

    /// <summary>
    /// ImGui用パラメータ読み込み
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void ParamLoadForImGui(const std::string& groupName, const std::string& fileName = "Application");

    /// <summary>
    /// グループ削除
    /// </summary>
    /// <param name="groupName">グループ名</param>
    void RemoveGroup(const std::string& groupName);

    /// <summary>
    /// グループの登録をクリア
    /// </summary>
    /// <param name="groupName"></param>
    void ClearRegistersForGroup(const std::string& groupName);

    // 全登録リセット
    void ResetAllRegister();

    // 全ファイル読み込み
    void LoadFiles();
    bool HasRegisters(const std::string& groupName) const;

    /// <summary>
    /// グループの登録変数値をdates_に書き込む
    /// </summary>
    void PushParamForGroup(const std::string& groupName);

    /// <summary>
    /// 指定プレフィックスで始まるキーをグループから削除
    /// </summary>
    void RemoveKeysWithPrefix(const std::string& groupName, const std::string& prefix);

private:
    std::unordered_map<std::string, Group> dates_;
    // 配列用のJSON配列
    std::unordered_map<std::string, std::unordered_map<std::string, json>> arrayDates_;
    std::unordered_map<std::string, std::vector<BoundItem>> registerParams_;
    const std::string kDirectoryPath = "resources/GlobalParameter/";

public:
    const std::string& GetDirectoryPath() const { return kDirectoryPath; }
};

}; // KetaEngine
