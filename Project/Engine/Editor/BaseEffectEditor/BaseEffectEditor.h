#pragma once
#include <memory>
#include <string>
#include <vector>

namespace KetaEngine {

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

    virtual void Init(const std::string& typeName);
    virtual void Update(float speedRate = 1.0f);
    virtual void EditorUpdate();

    // カテゴリー操作
    void AddCategory(const std::string& categoryName);
    void RemoveCategory(int32_t index);
    void AddEffectToCategory(int32_t categoryIndex, const std::string& effectName);
    void RemoveEffectFromCategory(int32_t categoryIndex, int32_t effectIndex);

    // セーブ、ロード
    void AllLoadFile();
    void AllSaveFile();
    void SaveCategory(int32_t categoryIndex);
    void LoadCategory(const std::string& categoryName);
    void RenderPlayBack();

protected:
    //*---------------------------- protected Methods ----------------------------*//

    // 派生クラスで実装必須
    virtual std::unique_ptr<TEffectData> CreateEffectData() = 0;
    virtual void RenderSpecificUI()                         = 0;
    virtual void PlaySelectedAnimation()                    = 0;
    virtual std::string GetFolderName() const               = 0;
 
private:
    // 共通UIレンダリング
    void RenderFileOperations();
    void RenderCategoryUI();
    void RenderCategoryEffectListUI();

    // 選択アニメーション一時停止、リセット
    void PauseSelectedAnimation();
    void ResetSelectedAnimation();

    // 選択アニメーション状態取得
    bool IsSelectedAnimationPlaying() const;
    bool IsSelectedAnimationFinished() const;

protected:
    //*---------------------------- protected Variant ----------------------------*//

    // カテゴリーモード用
    std::vector<Category> categories_;
    int32_t selectedCategoryIndex_ = -1;

    // 共通
    char nameBuffer_[128]         = "";
    char categoryNameBuffer_[128] = "";
    std::string baseFolderPath_;
    std::string effectTypeName_;

    bool isEditing_ = false;

private:
    const std::string datesFolderName_ = "Dates/";

public:
    //*----------------------------- getter Methods -----------------------------*//

    TEffectData* GetSelectedEffect();
    TEffectData* GetEffectByName(const std::string& categoryName, const std::string& effectName);

    bool GetIsEditing() const { return isEditing_; }
    const std::vector<Category>& GetCategories() const { return categories_; }
    int32_t GetCategoryCount() const { return static_cast<int32_t>(categories_.size()); }
};

}; // KetaEngine