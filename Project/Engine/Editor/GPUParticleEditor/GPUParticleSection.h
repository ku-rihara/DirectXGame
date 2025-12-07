#pragma once
#include "3d/WorldTransform.h"
#include "GPUParticle/Data/GPUParticleEmitterData.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "Line3D/Line3D.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Primitive/IPrimitive.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルセクション
/// </summary>
class GPUParticleSection {
public:
    enum class PlayState {
        STOPPED,
        WAITING,
        PLAYING
    };

    struct EmitParameters {
        Vector3 scaleMin         = {0.5f, 0.5f, 0.5f};
        Vector3 scaleMax         = {1.5f, 1.5f, 1.5f};
        Vector3 rotationMin      = {0.0f, 0.0f, 0.0f};
        Vector3 rotationMax      = {360.0f, 360.0f, 360.0f};
        Vector3 rotationSpeedMin = {0.0f, 0.0f, 0.0f};
        Vector3 rotationSpeedMax = {180.0f, 180.0f, 180.0f};
        Vector3 translateMin     = {-1.0f, -1.0f, -1.0f};
        Vector3 translateMax     = {1.0f, 1.0f, 1.0f};
        Vector3 velocityMin      = {-1.0f, -1.0f, -1.0f};
        Vector3 velocityMax      = {1.0f, 1.0f, 1.0f};
        Vector4 colorMin         = {1.0f, 1.0f, 1.0f, 1.0f};
        Vector4 colorMax         = {1.0f, 1.0f, 1.0f, 1.0f};
        float lifeTimeMin        = 1.0f;
        float lifeTimeMax        = 3.0f;
    };

    struct EmitterSettings {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        uint32_t count   = 10;
        float frequency  = 1.0f;
    };

    struct GroupSettings {
        BlendMode blendMode = BlendMode::Add;
        bool isActive       = true;
    };

public:
    GPUParticleSection()  = default;
    ~GPUParticleSection() = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex);
    void Update(float speedRate = 1.0f);
    void Reset();
    bool IsFinished() const;

    void LoadData();
    void SaveData();
    void AdjustParam();

    void StartWaiting();
    void Emit();
    void StartRailEmit();

private:
    //*---------------------------- private Methods ----------------------------*//
    void ParameterInit();
    void RegisterParams();
    void GetParams();
    void ApplyParameters();
    void ApplyTexture(const std::string& textureName);
    void AdaptTexture();

    void RailMoveUpdate();
    void UpdateEmitTransform();
    void SetEmitLine();

    void EmitParameterEditor();
    void EmitterSettingsEditor();
    void ImGuiTextureSelection();
    void ParticlePresetChange();

    void DisplayFileSelection(
        const std::string& header,
        const std::vector<std::string>& filenames,
        int& selectedIndex,
        const std::function<void(const std::string&)>& onApply);

    void StartPlay();
    void UpdateWaiting(float deltaTime);

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string sectionName_;   
    std::string particleName_;  
    std::string categoryName_;  
    std::string groupName_;     
    int32_t sectionIndex_ = -1;

    EmitParameters emitParams_;
    EmitterSettings emitterSettings_;
    GroupSettings groupSettings_;

    GlobalParameter* globalParameter_ = nullptr;
    std::unique_ptr<RailManager> railManager_;
    std::unique_ptr<Line3D> debugLine_;
    WorldTransform emitBoxTransform_;

    float currentTime_    = 0.0f;
    float elapsedTime_    = 0.0f;
    float startTime_      = 0.0f;
    bool shouldEmit_      = false;
    bool isStartRailMove_ = false;

    int blendModeIndex_ = 1;

    const std::string baseFolderPath_  = "GPUParticle/";
    const std::string textureFilePath_ = "Resources/texture";
    std::string folderPath_;
    std::string selectedTexturePath_;

    bool isMoveForRail_ = false;
    bool isRailLoop_    = false;
    float moveSpeed_    = 1.0f;

    PlayState playState_ = PlayState::STOPPED;
    TimeModeSelector timeModeSelector_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetSectionName() const { return sectionName_; }
    const EmitParameters& GetEmitParams() const { return emitParams_; }
    const EmitterSettings& GetEmitterSettings() const { return emitterSettings_; }
    const GroupSettings& GetGroupSettings() const { return groupSettings_; }

    //*----------------------------- setter Methods -----------------------------*//
    void SetTexture(uint32_t textureHandle);
    void SetEmitterData(const ParticleEmit& emitter);
    void SetParentBasePos(WorldTransform* parent) { emitBoxTransform_.parent_ = parent; }
};