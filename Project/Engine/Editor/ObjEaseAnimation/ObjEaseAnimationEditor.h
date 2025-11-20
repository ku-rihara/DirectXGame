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
    void Update();
    void EditorUpdate(); 

    //カテゴリー追加、削除
    void AddCategory(const std::string& categoryName);
    void RemoveCategory(const int32_t& index);

    // アニメーション追加、削除
    void AddAnimation(const int32_t& categoryIndex, const std::string& animationName);
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

    const std::string& objEaseAnimationBasePath_ = "Resources/GlobalParameter/ObjEaseAnimation/";
    const std::string& dateFolderName_           = "Dates/";

    // プレビュー設定
    bool showPreview_                 = true;
    char previewModelNameBuffer_[128] = "DebugCube.obj";
    PreViewTransform previewBaseTransform_;


public:
    const int32_t& GetCategoryCount() const { return static_cast<int32_t>(categories_.size()); }
    const std::vector<Category>& GetCategories() const { return categories_; }
};