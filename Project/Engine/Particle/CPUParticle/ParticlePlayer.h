#pragma once
#include "3d/WorldTransform.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "Editor/ParticleData.h"
#include "Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {

class Object3DAnimation;

/// <summary>
/// パーティクルプレイヤー
/// </summary>
class ParticlePlayer : public BaseEffectPlayer {
public:
    struct ParentParam {
        const Vector3* followPos_               = nullptr;
        const WorldTransform* transform_        = nullptr;
        const Object3DAnimation* modelAnimation = nullptr;
        std::string jointName;
    };

public:
    ParticlePlayer()           = default;
    ~ParticlePlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    /// <summary>
    /// パーティクルをEmitする
    /// </summary>
    void Play(const std::string& particleName, const std::string& categoryName = "Common") override;

    /// <summary>
    /// 再生状況
    /// </summary>
    void UpdatePlayState();

private:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;
    void ApplyParentParametersToData(ParticleData* particleData);

private:
    
    std::string currentParticleName_;
    bool isInitialized_          = false;
    bool wasPlayCalledThisFrame_ = false;

    ParentParam parentParam_;

public:
    ParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
    const std::string& GetCurrentParticleName() const { return currentParticleName_; }

    //*----------------------------- Runtime Settings  -----------------------------*//

    void SetTargetPosition(const Vector3& targetPos);
    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);
};

}