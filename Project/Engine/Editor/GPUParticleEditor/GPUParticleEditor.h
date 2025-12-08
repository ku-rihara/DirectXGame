#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "GPUParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルエディター
/// </summary>
class GPUParticleEditor : public BaseEffectEditor<GPUParticleData> {
public:
    GPUParticleEditor()  = default;
    ~GPUParticleEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& typeName, const bool& isUseCategory = false) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<GPUParticleData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;

    std::string GetCategoryFolderName() const override { return particleFolderName_; }
    std::string GetDataFolderName() const override { return dateFolderName_; }

private:
    //*---------------------------- private Variant ----------------------------*//
    const std::string particleFolderName_ = "GPUParticle/";
    const std::string dateFolderName_     = "Dates/";
};