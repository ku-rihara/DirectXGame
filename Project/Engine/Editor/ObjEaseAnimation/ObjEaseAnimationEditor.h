#pragma once
#include "ObjEaseAnimationData.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// オブジェクトイージングアニメーションエディター
/// </summary>
class ObjEaseAnimationEditor {
public:
    struct Category {
        std::string name;
        std::vector<std::unique_ptr<ObjEaseAnimationData>> animations;
        int32_t selectedAnimationIndex = -1;
    };

public:
    ObjEaseAnimationEditor()  = default;
    ~ObjEaseAnimationEditor() = default;

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    void EditorUpdate(); //< エディタ更新

    /// <summary>
    /// カテゴリーの追加
    /// </summary>
    /// <param name="categoryName">カテゴリー名</param>
    void AddCategory(const std::string& categoryName);

    /// <summary>
    /// カテゴリーの削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void RemoveCategory(const int32_t& index);

    /// <summary>
    /// アニメーションの追加
    /// </summary>
    /// <param name="categoryIndex">カテゴリーインデックス</param>
    /// <param name="animationName">アニメーション名</param>
    void AddAnimation(const int32_t& categoryIndex, const std::string& animationName);

    /// <summary>
    /// アニメーションの削除
    /// </summary>
    /// <param name="categoryIndex">カテゴリーインデックス</param>
    /// <param name="animationIndex">アニメーションインデックス</param>
    void RemoveAnimation(const int32_t& categoryIndex, const int32_t& animationIndex);

    /// <summary>
    /// 名前からアニメーションデータを取得
    /// </summary>
    /// <param name="categoryName">カテゴリー名</param>
    /// <param name="animationName">アニメーション名</param>
    /// <returns>アニメーションデータ</returns>
    ObjEaseAnimationData* GetAnimationByName(const std::string& categoryName, const std::string& animationName);

private:
    void AllLoadFile(); //< 全ファイルロード
    void AllSaveFile(); //< 全ファイルセーブ
    void LoadCategory(const std::string& categoryName); //< カテゴリーロード
    void SaveCategory(const int32_t& categoryIndex); //< カテゴリーセーブ

private:
    std::vector<Category> categories_;
    int32_t selectedCategoryIndex_ = -1;

    char categoryNameBuffer_[128]  = "";
    char animationNameBuffer_[128] = "";

public:
    const int32_t& GetCategoryCount() const { return static_cast<int32_t>(categories_.size()); }
    const std::vector<Category>& GetCategories() const { return categories_; }
};