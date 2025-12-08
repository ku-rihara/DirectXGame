#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "GPUParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルプレイヤー
/// </summary>
class GPUParticlePlayer : public BaseEffectPlayer {
public:
    GPUParticlePlayer()           = default;
    ~GPUParticlePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& particleName);

    // 描画
    void Draw();

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string currentCategoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    GPUParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};