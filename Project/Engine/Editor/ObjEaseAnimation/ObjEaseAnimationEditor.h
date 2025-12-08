#pragma once
#include "3d/Object3d.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ObjEaseAnimationData.h"
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションエディター
/// </summary>
class ObjEaseAnimationEditor : public BaseEffectEditor<ObjEaseAnimationData> {
public:
    struct PreViewTransform {
        Vector3 scale       = Vector3::OneVector();
        Vector3 rotation    = Vector3::ZeroVector();
        Vector3 translation = Vector3::ZeroVector();
    };

public:
    ObjEaseAnimationEditor()  = default;
    ~ObjEaseAnimationEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName, const bool& isUseCategory = false) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<ObjEaseAnimationData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;

    // カテゴリーシステムを使用
    std::string GetCategoryFolderName() const override { return animationFolderName_; }
    std::string GetDataFolderName() const override { return dateFolderName_; }

private:
    //*---------------------------- private Methods ----------------------------*//

    // Preview初期化、更新、切り替え
    void InitPreviewObject();
    void UpdatePreviewObject();
    void ChangePreviewModel(const std::string& modelName);

private:
    //*---------------------------- private Variant ----------------------------*//

    std::unique_ptr<Object3d> previewObject_ = nullptr;

    const std::string animationFolderName_ = "ObjEaseAnimation/";
    const std::string dateFolderName_      = "Dates/";

    // プレビュー設定
    bool showPreview_                 = true;
    char previewModelNameBuffer_[128] = "DebugCube.obj";
    PreViewTransform previewBaseTransform_;

public:
    //*----------------------------- getter Methods -----------------------------*//

};