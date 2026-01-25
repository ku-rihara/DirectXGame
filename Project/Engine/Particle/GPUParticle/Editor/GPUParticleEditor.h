#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "GPUParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルエディター
/// </summary>
namespace KetaEngine {

class GPUParticleEditor : public BaseEffectEditor<GPUParticleData> {
public:
    GPUParticleEditor()  = default;
    ~GPUParticleEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<GPUParticleData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderName() const override;


private:
    //*---------------------------- private Variant ----------------------------*//
    const std::string particleFolderName_ = "GPUParticle/";
   
};

}; // KetaEngine
