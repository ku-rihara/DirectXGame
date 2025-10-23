#pragma once
#include "3d/Object3d.h"
#include "DissolveData.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// ディゾルブエディタクラス
/// </summary>
class DissolveEditor {
public:
    DissolveEditor()  = default;
    ~DissolveEditor() = default;

    void Init();         //< 初期化
    void EditorUpdate(); //< エディタ更新

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

    /// <summary>
    /// ディゾルブ追加
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    void AddDissolve(const std::string& dissolveName);

    /// <summary>
    /// ディゾルブ削除
    /// </summary>
    /// <param name="index">削除するインデックス</param>
    void RemoveDissolve(int index);

    void AllLoadFile(); //< 全ファイル読み込み
    void AllSaveFile(); //< 全ファイル保存

    DissolveData* GetSelectedDissolve();                      //< 選択中のディゾルブ取得
    DissolveData* GetDissolveByName(const std::string& name); //< 名前でディゾルブ取得
    int GetDissolveCount() const { return static_cast<int>(dissolves_.size()); }

private:
    std::vector<std::unique_ptr<DissolveData>> dissolves_;
    std::unique_ptr<Object3d> preViewObj_;
    int selectedIndex_    = -1;
    char nameBuffer_[128] = "";
};