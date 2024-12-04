#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
/// std
#include <map>
#include <string>
#include <variant>
#include<stack>
#include <json.hpp>

class GlobalParameter {
public:
    // 描画設定用の構造体
    struct DrawSettings {
        enum class WidgetType {
            SliderInt,
            DragFloat,
            DragFloat2,
            DragFloat3,
            DragFloat4,
            Checkbox,
            ColorEdit4,   
        };

        WidgetType widgetType;       // 使用するImGuiウィジェットの種類
        float minValue = 0.0f;       // 最小値 (必要に応じて)
        float maxValue = 1.0f;       // 最大値 (必要に応じて)
        std::string treeNodeLabel;   // TreeNodeの場合のラベル (必要に応じて)
    };

private:

    /// 格納できる複数の値の型を定義
    using Item = std::variant
        <int32_t,uint32_t, float, Vector2, Vector3, Vector4, bool>;

    using Parameter = std::pair<Item, DrawSettings>; // 値と描画設定をペアにする
    using Group = std::map<std::string, Parameter>;  // パラメータ名とペアをマップに格納


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

    std::stack<std::string> treeNodeStack_;

public:

    static GlobalParameter* GetInstance();

    ///=================================================================================
   ///public method
   ///=================================================================================

    ///　　更新
    void Update();

    // 新しいグループを作成
    void CreateGroup(const std::string& groupName);

    // ツリーのノード追加
    void AddTreeNode(const std::string& nodeName);

    // ツリーのノードを閉じる
    void AddTreePoP();

    void DrawWidget(const std::string& itemName, Item& item, const DrawSettings& drawSettings);

    // 値を設定する
    template<typename T> void SetValue(const std::string& groupName, const std::string& key, T value, GlobalParameter::DrawSettings::WidgetType widgetType);

    // 新しいアイテムをグループに追加する
    template<typename T> void AddItem(const std::string& groupName, const std::string& key, T value, GlobalParameter::DrawSettings::WidgetType widgetType);

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