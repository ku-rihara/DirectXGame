// GlobalParameter.h

#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
/// std
#include <functional>
#include <json.hpp>
#include <map>
#include <stack>
#include <string>
#include <variant>
#include <vector>

class GlobalParameter {

public:
    ///=================================================================================
    /// WitgetType
    ///=================================================================================

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

    ///=================================================================================
    /// Draw Settings
    ///=================================================================================

    struct DrawSettings {
        WidgetType widgetType;
        float minValue = 0.0f;
        float maxValue = 100.0f;
        std::string treeNodeLabel;
    };

private:
    ///=================================================================================
    /// BoundItem
    ///=================================================================================

    struct BoundItem {
        std::function<void()> pullVariant;
        std::function<void()> pushVariant;
    };

private:
    
    /// usisng

    using Item = std::variant<int32_t, uint32_t, float, Vector2, Vector3, Vector4, bool, std::string>;

    using Parameter = std::pair<Item, DrawSettings>; // 値と描画設定をペアにする
    using Group     = std::map<std::string, Parameter>; // パラメータ名とペア

    /// json
    using json = nlohmann::json;

public:
    static GlobalParameter* GetInstance();
    GlobalParameter() = default;
    ~GlobalParameter() = default;

    ///=================================================================================
    /// public method
    ///=================================================================================

    /// 更新
    void Update();

    // 新しいグループを作成
    void CreateGroup(const std::string& groupName, const bool& isVisible);

    // ツリーのノード追加
    void AddSeparatorText(const std::string& nodeName);

    // ツリーのノードを閉じる
    void AddTreePoP();

    // 値を設定する
    template <typename T>
    void SetValue(const std::string& groupName, const std::string& key, T value, WidgetType widgetType = WidgetType::NONE);

    // 新しいアイテムをグループに追加する
    template <typename T>
    void AddItem(const std::string& groupName, const std::string& key, T value, WidgetType widgetType = WidgetType::NONE);

    // 値を取得する
    template <typename T>
    T GetValue(const std::string& groupName, const std::string& key) const;

    // draw
    void DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings);

    template <typename T>
    void Bind(const std::string& group, const std::string& key, T* variable, WidgetType widgetType = WidgetType::NONE);
    void BindResetAll();
    void CopyGroup(const std::string& fromGroup, const std::string& toGroup);

    void SyncAll();
    void SyncParamForGroup(const std::string& group);

    // ------------------------------------------------------------------------------
    // ファイルへの保存・読み込み
    // ------------------------------------------------------------------------------

    // すべてのグループのデータをファイルから読み込む
    void LoadFiles();

    // 特定のグループのデータをファイルから読み込む
    void LoadFile(const std::string& groupName, const std::string& fileName = "");
    void SaveFile(const std::string& groupName, const std::string& fileName = "");

    void ParamSaveForImGui(const std::string& groupName, const std::string& fileName = "");
    void ParamLoadForImGui(const std::string& groupName, const std::string& fileName = "");

    void ClearBindingsForGroup(const std::string& groupName);
    void RemoveGroup(const std::string& groupName);

private:
    ///=================================================================================
    /// private variant
    ///=================================================================================

    // グループ名(キー)とグループのデータ
    std::unordered_map<std::string, Group> dates_;

    /// グループごとの可視性フラグを管理
    std::unordered_map<std::string, bool> visibilityFlags_;


    std::unordered_map<std::string, std::vector<BoundItem>> bindings_;

    // データを保存する際のディレクトリパス
    const std::string kDirectoryPath = "Resources/GlobalParameter/";

    std::stack<std::string> treeNodeStack_;
};