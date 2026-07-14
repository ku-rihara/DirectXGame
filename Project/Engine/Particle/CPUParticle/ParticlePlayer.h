#pragma once
#include "3D/WorldTransform.h"
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

    /// <summary>
    /// TargetPosition 用パラメータ
    /// pos_ / rotate_ にポインタをセットしておくと Update() で毎フレーム自動適用される
    /// </summary>
    struct TargetParam {
        const Vector3* pos_     = nullptr;  ///< 毎フレーム参照する TargetPosition ポインタ
        const Vector3* rotate_  = nullptr;  ///< 毎フレーム参照する TargetRotation ポインタ
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
    /// 優先パーティクルをEmitする
    /// </summary>
    void PlayPriority(const std::string& particleName, const std::string& categoryName = "Common");

    /// <summary>
    /// 再生状況
    /// </summary>
    void UpdatePlayState();

private:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;
    void ApplyParentParametersToData(ParticleData* particleData);
    void PlayInternal(const std::string& particleName, const std::string& categoryName, bool isPriority);

private:

    std::string currentParticleName_;
    bool isInitialized_          = false;
    bool wasPlayCalledThisFrame_ = false;

    // 優先エフェクトが通常Emitに即座に上書きされないためのガード
    bool currentIsPriority_       = false;
    float priorityGuardTimer_     = 0.0f;
    static constexpr float kPriorityGuardDuration = 0.3f;

    ParentParam parentParam_;
    TargetParam targetParam_;

    // 名前変更時に古いエフェクトを即破棄せず、afterDurationまで継続させるリスト
    std::vector<std::unique_ptr<BaseEffectData>> finishingEffects_;

public:
    ParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
    const std::string& GetCurrentParticleName() const { return currentParticleName_; }

    //*----------------------------- Runtime Settings  -----------------------------*//

    // --- Parent ---
    void SetParentTransform(const WorldTransform* transform);
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& jointName);
    void SetFollowingPos(const Vector3* pos);

    // --- TargetPosition (値渡し：1回だけ適用) ---
    void SetTargetPosition(const Vector3& targetPos);
    void SetTargetRotation(const Vector3& targetRotate);

    // --- TargetPosition (ポインタ渡し：Update で毎フレーム自動適用) ---
    void SetTargetPosPtr(const Vector3* pos);
    void SetTargetRotatePtr(const Vector3* rotate);

    // --- GoalPosition (ゴール誘導) ---
    void SetGoalPosition(const Vector3& pos);
};

}