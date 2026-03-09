#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "PostEffectData.h"
#include <memory>
#include <string>

/// <summary>
/// ポストエフェクトエディター
/// </summary>
namespace KetaEngine {

class PostEffectEditor : public BaseEffectEditor<PostEffectData> {
public:
    PostEffectEditor()  = default;
    ~PostEffectEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName) override;

    /// <summary>
    /// 名前でポストエフェクトを再生
    /// </summary>
    /// <param name="effectName">エフェクト名</param>
    /// <param name="categoryName">カテゴリー名</param>
    void PlayPostEffect(const std::string& effectName, const std::string& categoryName = "Common");

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<PostEffectData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;
    std::string GetFolderName() const override;
};

} // namespace KetaEngine
