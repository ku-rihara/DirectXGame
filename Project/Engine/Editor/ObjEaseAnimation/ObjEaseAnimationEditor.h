#pragma once
#include "ObjEaseAnimationData.h"
#include "3d/Object3d.h"
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

    struct PreViewTransform {
        Vector3 scale       = Vector3::OneVector();
        Vector3 rotation    = Vector3::ZeroVector();
        Vector3 translation = Vector3::ZeroVector();
    };
    ;

public:
    ObjEaseAnimationEditor()  = default;
    ~ObjEaseAnimationEditor() = default;

    // 初期化、更新
    void Init(); 
    void Update(const float& deltaTime);
    void EditorUpdate(); 

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

    // プレビューオブジェクト初期化、更新、モデル変更
   void InitPreviewObject();
   void UpdatePreviewObject();
   void ChangePreviewModel(const std::string& modelName);

    // 全カテゴリーのロード、セーブ
    void AllLoadFile(); 
    void AllSaveFile(); 
    void LoadCategory(const std::string& categoryName);
    void SaveCategory(const int32_t& categoryIndex);

private:
    std::vector<Category> categories_;
    int32_t selectedCategoryIndex_ = -1;
    std::unique_ptr<Object3d> previewObject_ = nullptr;

    char categoryNameBuffer_[128]  = "";
    char animationNameBuffer_[128] = "";

    // プレビュー設定
    bool showPreview_                 = true;
    char previewModelNameBuffer_[128] = "DebugCube.obj";
    PreViewTransform previewBaseTransform_;


public:
    const int32_t& GetCategoryCount() const { return static_cast<int32_t>(categories_.size()); }
    const std::vector<Category>& GetCategories() const { return categories_; }
};