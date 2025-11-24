#pragma once
#include <memory>
#include <string>
#include <vector>

template <typename TEffectData>
class BaseEffectEditor;

/// <summary>
/// エフェクトエディター基底クラス
/// </summary>
template <typename TEffectData>
class BaseEffectEditor {
public:
    // カテゴリー構造体
    struct Category {
        std::string name;
        std::vector<std::unique_ptr<TEffectData>> effects;
        int32_t selectedEffectIndex = -1;
    };

    virtual ~BaseEffectEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    virtual void Init(const std::string& typeName, const bool& isUseCategory = false);
    virtual void Update(const float& deltaTime);
    virtual void EditorUpdate();

    // 共通操作
    void AddEffect(const std::string& name);
    void RemoveEffect(const int32_t& index);

    // カテゴリー操作
    void AddCategory(const std::string& categoryName);
    void RemoveCategory(const int32_t& index);
    void AddEffectToCategory(const int32_t& categoryIndex, const std::string& effectName);
    void RemoveEffectFromCategory(const int32_t& categoryIndex, const int32_t& effectIndex);

    // セーブ、ロード
    void AllLoadFile();
    void AllSaveFile();
    void SaveCategory(const int32_t& categoryIndex);
    void LoadCategory(const std::string& categoryName);

protected:
    //*---------------------------- protected Methods ----------------------------*//

    // 派生クラスで実装必須
    virtual std::unique_ptr<TEffectData> CreateEffectData() = 0;
    virtual void RenderSpecificUI()                         = 0;
    virtual std::string GetFolderPath() const               = 0;

    // カテゴリー機能を使用するかどうか
    virtual std::string GetCategoryFolderName() const { return ""; }
    virtual std::string GetDataFolderName() const { return ""; }

    // 共通UIレンダリング
    void RenderEffectList();
    void RenderFileOperations();
    void RenderCategoryUI();
    void RenderCategoryEffectListUI();

protected:
    //*---------------------------- protected Variant ----------------------------*//

    // 非カテゴリーモード用
    std::vector<std::unique_ptr<TEffectData>> effects_;
    int32_t selectedIndex_ = -1;

    // カテゴリーモード用
    std::vector<Category> categories_;
    int32_t selectedCategoryIndex_ = -1;

    // 共通
    char nameBuffer_[128]         = "";
    char categoryNameBuffer_[128] = "";
    std::string baseFolderPath_;
    std::string effectTypeName_;

private:
    bool isUseCategorySystem_ = false;

public:
    //*----------------------------- getter Methods -----------------------------*//

    TEffectData* GetSelectedEffect();
    TEffectData* GetEffectByName(const std::string& name);

    // カテゴリーモード用getter
    TEffectData* GetEffectByName(const std::string& categoryName, const std::string& effectName);
    const std::vector<Category>& GetCategories() const { return categories_; }
    int32_t GetCategoryCount() const { return static_cast<int32_t>(categories_.size()); }
};