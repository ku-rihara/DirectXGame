#pragma once
#include "2d/Sprite.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "SpriteEaseAnimationData.h"
#include <memory>
#include <string>

/// <summary>
/// スプライトイージングアニメーションエディター
/// </summary>
namespace KetaEngine {

class SpriteEaseAnimationEditor : public BaseEffectEditor<SpriteEaseAnimationData> {
public:
    struct PreViewTransform {
        Vector2 scale    = Vector2::OneVector();
        Vector2 position = Vector2::ZeroVector();
        Vector3 rotation = Vector3::ZeroVector();
        Vector3 color    = {1.0f, 1.0f, 1.0f};
        float alpha      = 1.0f;
    };

public:
    SpriteEaseAnimationEditor()  = default;
    ~SpriteEaseAnimationEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
    void PlaySelectedAnimation() override;

    std::unique_ptr<SpriteEaseAnimationData> CreateEffectData() override;
    void RenderSpecificUI() override;

private:
    //*---------------------------- private Methods ----------------------------*//

    void InitPreviewSprite();
    void UpdatePreviewSprite();
    void ChangePreviewTexture(const std::string& textureName);

private:
    //*---------------------------- private Variant ----------------------------*//

    Sprite* previewSprite_           = nullptr;
    const std::string folderName_    = "SpriteEaseAnimation/";

    // プレビュー設定
    bool showPreview_                     = false;
    char previewTextureNameBuffer_[128]   = "UVChecker.dds";
    PreViewTransform previewBaseTransform_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    std::string GetFolderName() const override;
};

}; // KetaEngine
