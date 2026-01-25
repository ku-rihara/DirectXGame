#pragma once
#include "3d/Object3d/Object3d.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ObjEaseAnimationData.h"
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションエディター
/// カテゴリーシステムを使用
/// </summary>
namespace KetaEngine {

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
    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;


    std::unique_ptr<ObjEaseAnimationData> CreateEffectData() override;
    void RenderSpecificUI() override;

private:
    //*---------------------------- private Methods ----------------------------*//

    // Preview初期化、更新、切り替え
    void InitPreviewObject();
    void UpdatePreviewObject();
    void ChangePreviewModel(const std::string& modelName);

private:
    //*---------------------------- private Variant ----------------------------*//

    std::unique_ptr<Object3d> previewObject_ = nullptr;
    const std::string folderName_            = "ObjEaseAnimation/";

    // プレビュー設定
    bool showPreview_                 = true;
    char previewModelNameBuffer_[128] = "DebugCube.obj";
    PreViewTransform previewBaseTransform_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    std::string GetFolderName() const override;
    
};

}; // KetaEngine