#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ParticleData.h"
#include <memory>
#include <string>

namespace KetaEngine {

class Object3DAnimation;

/// <summary>
/// パーティクルプレイヤー
/// </summary>
class ParticlePlayer : public BaseEffectPlayer {
public:
    ParticlePlayer()           = default;
    ~ParticlePlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& particleName);

private:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    std::string currentCategoryName_;

public:
    ParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }

     //*----------------------------- Runtime Settings  -----------------------------*//

    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);
    void SetParentBasePos(WorldTransform* parent);
    void SetTargetPosition(const Vector3& targetPos);

    //*----------------------------- Section-Specific Settings -----------------------------*//

    void SetSectionParentTransform(int32_t sectionIndex, const WorldTransform* transform);
    void SetSectionParentJoint(int32_t sectionIndex, const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetSectionFollowingPos(int32_t sectionIndex, const Vector3* pos);
};

}