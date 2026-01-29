#pragma once
#include "Editor/BaseEffectEditor/BaseSequenceEffectData.h"
#include "ParticleSection.h"
#include <memory>
#include <string>
#include <vector>

namespace KetaEngine {

class Object3DAnimation;
class WorldTransform;

class ParticleData : public BaseSequenceEffectData<ParticleSection> {

public:
    // Parent parameter 
    struct ParentParam {
        const Vector3* followPos_               = nullptr;
        const WorldTransform* transform_        = nullptr;
        const Object3DAnimation* modelAnimation = nullptr;
        std::string jointName;
    };

public:
    ParticleData()           = default;
    ~ParticleData() override = default;

    // BaseEffectDataからのオーバーライド
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // カテゴリー対応初期化
    void Init(const std::string& name, const std::string& categoryName) override;

    // 編集用メソッド
    void AdjustParam();
  
    //  再生継続時間チェック機能
    void CheckAndPauseSectionsAfterDuration(float deltaTime);

    // Parent parameter application
    void ApplyParentParameters();

protected:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // BaseSequenceEffectDataからのオーバーライド
    void UpdateKeyFrameProgression() override;
    void AdvanceToNextSequenceElement() override;
    std::unique_ptr<ParticleSection> CreateKeyFrame(int32_t index) override;
    std::string GetSequenceElementFolderPath() const override;

private:
    void UpdateActiveSections(float speedRate);

private:
    const std::string baseFolderPath_ = "Particle/";
    float playSpeed_;
    float afterPlayTime_;

    bool isPlayByEditor_ = false;

    // Parent parameters
    ParentParam parentParam_;

public:
    const std::string& GetCategoryName() const { return categoryName_; }

    // セクション要素へのアクセス
    std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() {
        return sectionElements_;
    }
    const std::vector<std::unique_ptr<ParticleSection>>& GetSectionElements() const {
        return sectionElements_;
    }

    void SetIsPlayByEditor(bool is);

    // Parent parameter 
    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);
    void SetTargetPosition(const Vector3& targetPos);
};

}