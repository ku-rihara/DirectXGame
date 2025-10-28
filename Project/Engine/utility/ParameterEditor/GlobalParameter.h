#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <functional>
#include <json.hpp>
#include <map>
#include <stack>
#include <string>
#include <variant>
#include <vector>

/// <summary>
/// グローバルパラメータ管理クラス
/// </summary>
class GlobalParameter {
public:
    // ウィジェットタイプ
    enum class WidgetType {
        NONE,
        SliderInt,
        DragFloat,
        DragFloat2,
        DragFloat3,
        DragFloat4,
        Checkbox,
        ColorEdit4,
        SlideAngle,
    };

    // 描画設定
    struct DrawSettings {
        WidgetType widgetType;
        float minValue = 0.0f;
        float maxValue = 100.0f;
        std::string treeNodeLabel;
    };

private:
    struct BoundItem {
        std::function<void()> pullVariant;
        std::function<void()> pushVariant;
    };

    using Item      = std::variant<int32_t, uint32_t, float, Vector2, Vector3, Vector4, bool, std::string>;
    using Parameter = std::pair<Item, DrawSettings>;
    using Group     = std::map<std::string, Parameter>;
    using json      = nlohmann::json;

public:
    static GlobalParameter* GetInstance();
    GlobalParameter()  = default;
    ~GlobalParameter() = default;

    void Update(); //< 更新

    /// <summary>
    /// グループ作成
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="isVisible">表示フラグ</param>
    void CreateGroup(const std::string& groupName, const bool& isVisible);

    /// <summary>
    /// セパレータテキスト追加
    /// </summary>
    /// <param name="nodeName">ノード名</param>
    void AddSeparatorText(const std::string& nodeName);

    /// <summary>
    /// 値設定
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="value">値</param>
    /// <param name="widgetType">ウィジェットタイプ</param>
    template <typename T>
    void SetValue(const std::string& groupName, const std::string& key, T value, WidgetType widgetType = WidgetType::NONE);

    /// <summary>
    /// アイテム追加
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="value">値</param>
    /// <param name="widgetType">ウィジェットタイプ</param>
    template <typename T>
    void AddItem(const std::string& groupName, const std::string& key, T value, WidgetType widgetType = WidgetType::NONE);

    /// <summary>
    /// 値取得
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="key">キー</param>
    template <typename T>
    T GetValue(const std::string& groupName, const std::string& key) const;

    /// <summary>
    /// ウィジェット描画
    /// </summary>
    /// <param name="itemName">アイテム名</param>
    /// <param name="item">アイテム</param>
    /// <param name="drawSettings">描画設定</param>
    void DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings);

    /// <summary>
    /// 変数バインド
    /// </summary>
    /// <param name="group">グループ名</param>
    /// <param name="key">キー</param>
    /// <param name="variable">変数ポインタ</param>
    /// <param name="widgetType">ウィジェットタイプ</param>
    template <typename T>
    void Bind(const std::string& group, const std::string& key, T* variable, WidgetType widgetType = WidgetType::NONE);

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
    void LoadFile(const std::string& groupName, const std::string& fileName = "");

    /// <summary>
    /// ファイル保存
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void SaveFile(const std::string& groupName, const std::string& fileName = "");

    /// <summary>
    /// ImGui用パラメータ保存
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void ParamSaveForImGui(const std::string& groupName, const std::string& fileName = "");

    /// <summary>
    /// ImGui用パラメータ読み込み
    /// </summary>
    /// <param name="groupName">グループ名</param>
    /// <param name="fileName">ファイル名</param>
    void ParamLoadForImGui(const std::string& groupName, const std::string& fileName = "");

    /// <summary>
    /// グループ削除
    /// </summary>
    /// <param name="groupName">グループ名</param>
    void RemoveGroup(const std::string& groupName);

    /// グループにバインドが存在するか確認
    /// </summary>
    bool HasBindings(const std::string& groupName) const;

    void AddTreePoP(); //< ツリーポップ
    void BindResetAll(); //< 全バインドリセット
    void SyncAll(); //< 全同期
    void LoadFiles(); //< 全ファイル読み込み

private:
    std::unordered_map<std::string, Group> dates_;
    std::unordered_map<std::string, bool> visibilityFlags_;
    std::unordered_map<std::string, std::vector<BoundItem>> bindings_;
    const std::string kDirectoryPath = "Resources/GlobalParameter/";
    std::stack<std::string> treeNodeStack_;
};