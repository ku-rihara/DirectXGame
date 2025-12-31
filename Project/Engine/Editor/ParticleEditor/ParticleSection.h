#pragma once
#include "3d/WorldTransform.h"
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "Line3D/Line3D.h"
#include "ParticleSectionParameter.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "utility/FileSelector/FileSelector.h"
#include <memory>
#include <string>

namespace KetaEngine {

class Object3DAnimation;

/// <summary>
/// パーティクルセクション
/// </summary>
class ParticleSection {
public:
    enum class PlayState {
        STOPPED, // 停止中
        WAITING, // 開始待機中
        PLAYING // 再生中
    };

public:
    ParticleSection()  = default;
    ~ParticleSection() = default;

    //*----------------------------- Lifecycle Methods -----------------------------*//

    void Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex);
    void Update(float speedRate = 1.0f);
    void Reset();
    bool IsFinished() const;

    //*----------------------------- Playback Control -----------------------------*//

    void Play();
    void Pause();
    void Stop();
    void StartWaiting();

    //*----------------------------- Editor Methods -----------------------------*//

    void LoadData();
    void SaveData();
    void AdjustParam();

    //*----------------------------- Setup Methods -----------------------------*//

    void CreateModelParticle(const std::string& modelFilePath, int32_t maxnum);
    void CreatePrimitiveParticle(PrimitiveType primitiveType, int32_t maxnum);
    void ChangePrimitive(const PrimitiveType& primitiveType);
    void SetTextureHandle(uint32_t handle);
    void InitAdaptTexture();

private:
    //*---------------------------- Internal Methods ----------------------------*//

    void ApplyTextureToManager();
    void AdaptEaseSettings();
    void AdaptRailSettings();
    void UpdateEmitTransform();
    void SetEmitLine();
    void StartPlay();
    void UpdateWaiting(float deltaTime);
    void EmitInternal();

private:
    //*---------------------------- Member Variables ----------------------------*//

    std::string particleName_;
    std::string categoryName_;
    std::string groupName_;
    int32_t sectionIndex_ = -1;

    // パラメータ
    ParticleSectionParameter sectionParam_;

    // スケールイージング用
    Easing<Vector3> scaleEasing_;

    GlobalParameter* globalParameter_ = nullptr;
    std::unique_ptr<RailPlayer> railPlayer_;
    std::unique_ptr<Line3D> debugLine_;
    WorldTransform emitBoxTransform_;

    float currentTime_ = 0.0f; // Emit間隔タイマー
    float elapsedTime_ = 0.0f; // 待機時間タイマー

    const std::string baseFolderPath_ = "Particle/";
    const std::string modelBasePath_  = "Resources/Model/";
    std::string folderPath_;

    // Model関連
    FileSelector modelFileSelector_;

    PlayState playState_ = PlayState::STOPPED;

public:
    //*----------------------------- Getters -----------------------------*//

    const std::string& GetGroupName() const { return groupName_; }
    ParticleSectionParameter::Parameters& GetParameters() { return sectionParam_.GetParameters(); }
    ParticleSectionParameter::GroupParamaters& GetGroupParameters() { return sectionParam_.GetGroupParameters(); }
    int32_t GetParticleCount() const { return sectionParam_.GetParticleCount(); }
    float GetIntervalTime() const { return sectionParam_.GetIntervalTime(); }
    int32_t GetMaxParticleNum() const { return sectionParam_.GetMaxParticleNum(); }
    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
    PlayState GetPlayState() const { return playState_; }
    bool IsPlaying() const { return playState_ == PlayState::PLAYING; }
    ParticleSectionParameter& GetSectionParam() { return sectionParam_; }
    const ParticleSectionParameter& GetSectionParam() const { return sectionParam_; }

    //*----------------------------- Setters -----------------------------*//

    void SetTexture(uint32_t textureHandle);
    void SetParentBasePos(WorldTransform* parent) { emitBoxTransform_.parent_ = parent; }
};

}; // KetaEngine