#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ParticleData.h"
#include "ParticleSection.h"
#include <memory>
#include <string>

namespace KetaEngine {

class ParticleEditor : public BaseEffectEditor<ParticleData> {
public:
    ParticleEditor()  = default;
    ~ParticleEditor() = default;

    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;
    void PlaySelectedAnimation() override;

protected:
    std::unique_ptr<ParticleData> CreateEffectData() override;
    void RenderSpecificUI() override;

private:
    /// <summary>
    /// 全セクションをプレビュー再生
    /// </summary>
    void PreviewAllSections(ParticleData* particleData);

    /// <summary>
    /// 全セクションを停止
    /// </summary>
    void StopAllSections(ParticleData* particleData);

    /// <summary>
    /// 全パーティクルグループを初期化
    /// </summary>
    void InitializeAllParticleGroups();

    /// <summary>
    /// 指定したParticleDataの全セクションのパーティクルグループを初期化
    /// </summary>
    void InitializeParticleDataGroups(ParticleData* particleData);

    /// <summary>
    /// 指定したセクションのパーティクルグループを初期化
    /// </summary>
    void InitializeSectionParticleGroup(ParticleSection* section);

private:
    const std::string particleFolderName_ = "Particle/";
  
public:
    std::string GetFolderName() const override;

};

}