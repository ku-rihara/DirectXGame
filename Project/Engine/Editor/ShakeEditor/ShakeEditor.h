#pragma once
#include "3d/Object3d/Object3d.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ShakeData.h"
#include <memory>
#include <string>

/// <summary>
/// シェイクエディター
/// カテゴリーシステムを使用
/// </summary>
namespace KetaEngine {

class ShakeEditor : public BaseEffectEditor<ShakeData> {
public:
    ShakeEditor()  = default;
    ~ShakeEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
  
protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<ShakeData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;

private:
    //*---------------------------- private Methods ----------------------------*//

    // プレビュー更新
    void UpdatePreviewObject(float speedRate);

private:
    //*---------------------------- private Variant ----------------------------*//

    std::unique_ptr<Object3d> preViewObj_;
    const std::string folderName_ = "ShakeEditor/";

    // プレビュー設定
    bool showPreview_ = false;
    Vector3 basePos_  = Vector3::ZeroVector();

public:
    //*----------------------------- getter Methods -----------------------------*//
    std::string GetFolderName() const override;
};

}; // KetaEngine