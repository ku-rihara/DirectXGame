#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ParticleData.h"
#include <memory>
#include <string>

class Object3DAnimation;

/// <summary>
/// パーティクルプレイヤー
/// </summary>
class ParticlePlayer : public BaseEffectPlayer {
public:
    ParticlePlayer()           = default;
    ~ParticlePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& particleName);

    // 描画
    void Draw();

    //*----------------------------- Runtime Settings -----------------------------*//


    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);
    void SetParentBasePos(WorldTransform* parent);
    void SetTargetPosition(const Vector3& targetPos);
    void SetSectionParentTransform(int32_t sectionIndex, const WorldTransform* transform);
    void SetSectionParentJoint(int32_t sectionIndex, const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetSectionFollowingPos(int32_t sectionIndex, const Vector3* pos);

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string currentCategoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    ParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};