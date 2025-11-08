#pragma once
#include "3d/Object3d.h"
#include "RailData.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// レールエディター
/// </summary>
class RailEditor {
public:
    RailEditor()  = default;
    ~RailEditor() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// レールの追加
    /// </summary>
    /// <param name="railName">レール名</param>
    void AddRail(const std::string& railName);

    /// <summary>
    /// レールの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemoveRail(const int& index);

    void EditorUpdate(); //< エディタ更新

    RailData* GetSelectedRail(); //< 選択中のレール取得

    /// <summary>
    /// 名前からレールデータを取得
    /// </summary>
    /// <param name="name">レール名</param>
    /// <returns>レールデータ</returns>
    RailData* GetRailByName(const std::string& name);

private:
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ

private:
    std::vector<std::unique_ptr<RailData>> rails_;
    std::unique_ptr<Object3d> preViewObj_;

    Vector3 basePos_   = Vector3::ZeroVector();
    int selectedIndex_ = -1;

    bool isPreViewDraw_ = false;

    char nameBuffer_[128] = "";

public:
    const int& GetRailCount() const { return static_cast<int>(rails_.size()); }
};