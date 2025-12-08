#pragma once
#include "3d/WorldTransform.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "GPUParticle/Data/GPUParticleEmitterData.h"
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

    enum class BillboardMode {
        None      = 0,
        Billboard = 1,
        YAxis     = 2
    };

public:
    struct UVParameters {
        Vector2 uvPosition;
        float uvRotate;
        Vector2 uvScrollSpeed;
        Vector2 uvScale = Vector2::OneVector();
        bool isFlipX;
        bool isFlipY;
        float pixelStep;
        bool isUVLoop = true;
    };

    struct TransformParameters {
        Vector3 scaleMin = {0.5f, 0.5f, 0.5f};
        Vector3 scaleMax = {1.5f, 1.5f, 1.5f};
        Vector3 rotationMin;
        Vector3 rotationMax;
        Vector3 translateMin;
        Vector3 translateMax;
    };

    struct PhysicsParameters {
        Vector3 velocityMin;
        Vector3 velocityMax;
        Vector3 rotationSpeedMin;
        Vector3 rotationSpeedMax;
    };

    struct AppearanceParameters {
        Vector4 colorMin;
        Vector4 colorMax;
        float lifeTimeMin;
        float lifeTimeMax;
    };

    struct EmitterSettings {
        Vector3 position;
        uint32_t count  = 10;
        float frequency = 1.0f;
    };

    struct GroupSettings {
        BlendMode blendMode   = BlendMode::Add;
        bool isActive         = true;
        bool alignToVelocity = true;
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

    TransformParameters transformParams_;
    PhysicsParameters physicsParams_;
    AppearanceParameters appearanceParams_;

    EmitterSettings emitterSettings_;
    GroupSettings groupSettings_;

    int32_t billboardModeInt_        = 1;
    BillboardMode billboardMode_ = BillboardMode::Billboard;
    UVParameters uvParams_;

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
    const EmitterSettings& GetEmitterSettings() const { return emitterSettings_; }
    const GroupSettings& GetGroupSettings() const { return groupSettings_; }

    //*----------------------------- setter Methods -----------------------------*//
    void SetTexture(uint32_t textureHandle);
    void SetEmitterData(const ParticleEmit& emitter);
    void SetParentBasePos(WorldTransform* parent) { emitBoxTransform_.parent_ = parent; }
};