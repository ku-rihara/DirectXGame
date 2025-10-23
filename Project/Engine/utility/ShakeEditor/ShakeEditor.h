#pragma once
#include "3d/Object3d.h"
#include "ShakeData.h"
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// シェイクエディター
/// </summary>
class ShakeEditor {
public:
    ShakeEditor()  = default;
    ~ShakeEditor() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// シェイクの追加
    /// </summary>
    /// <param name="shakeName">シェイク名</param>
    void AddShake(const std::string& shakeName);

    /// <summary>
    /// シェイクの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemoveShake(const int& index);

    void EditorUpdate(); //< エディタ更新
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ

    ShakeData* GetSelectedShake(); //< 選択中のシェイク取得

    /// <summary>
    /// 名前からシェイクデータを取得
    /// </summary>
    /// <param name="name">シェイク名</param>
    /// <returns>シェイクデータ</returns>
    ShakeData* GetShakeByName(const std::string& name);

private:
    std::vector<std::unique_ptr<ShakeData>> shakes_;
    std::unique_ptr<Object3d> preViewObj_;

    Vector3 basePos_   = Vector3::ZeroVector();
    int selectedIndex_ = -1;

    bool isPreViewDraw_;

    char nameBuffer_[128] = "";

public:
    const int& GetShakeCount() const { return static_cast<int>(shakes_.size()); }
};