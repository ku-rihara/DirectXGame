#pragma once
#include "3d/WorldTransform.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "Editor/GPUParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルプレイヤー
/// </summary>
namespace KetaEngine {

class GPUParticlePlayer : public BaseEffectPlayer {
public:
    GPUParticlePlayer()           = default;
    ~GPUParticlePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;

   
    void InitEffect(const std::string& categoryName, const std::string& particleName);
    void Update(float speedRate = 1.0f) override;
    void Play(const std::string& categoryName, const std::string& particleName) override;
 
    void Reset();
    void Draw();

    // トランスフォーム設定
    void SetEmitPosition(const Vector3& position);
    void SetParentTransform(WorldTransform* parent);

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    
    bool isInitialized_          = false;
    bool playRequestedThisFrame_ = false;

public:
    //*----------------------------- getter Methods -----------------------------*//
    GPUParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
    bool IsInitialized() const { return isInitialized_; }
};

}; // KetaEngine