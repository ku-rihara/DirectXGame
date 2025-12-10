#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// パーティクルエディター
/// </summary>
namespace KetaEngine {

class ParticleEditor : public BaseEffectEditor<ParticleData> {
public:
    ParticleEditor()  = default;
    ~ParticleEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& typeName, const bool& isUseCategory = false) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<ParticleData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;

    std::string GetCategoryFolderName() const override { return particleFolderName_; }
    std::string GetDataFolderName() const override { return dateFolderName_; }

private:
    //*---------------------------- private Variant ----------------------------*//
    const std::string particleFolderName_ = "Particle/";
    const std::string dateFolderName_     = "Dates/";
};

}; // KetaEngine
