#pragma once

#include "3d/WorldTransform.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "Data/GPUParticleEmitterData.h"
#include "Line3D/Line3D.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Primitive/IPrimitive.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルエミッタークラス
/// </summary>
class GPUParticleEmitter {
public:
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
        float radius     = 1.0f;
        uint32_t count   = 10;
        float frequency  = 1.0f;
    };

    struct GroupSettings {
        BlendMode blendMode = BlendMode::Add;
        bool isActive       = true;
    };

public:
    GPUParticleEmitter()  = default;
    ~GPUParticleEmitter() = default;

    /// <summary>
    /// パーティクル作成
    /// </summary>
    static GPUParticleEmitter* CreateParticle(
        const std::string& name,
        const std::string& modelFilePath,
        int32_t maxCount);

    /// <summary>
    /// パーティクル作成(Primitive)
    /// </summary>
    static GPUParticleEmitter* CreateParticlePrimitive(
        const std::string& name,
        const PrimitiveType& type,
        int32_t maxCount);

    /// <summary>
    /// 初期化
    /// </summary>
    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// エミット
    /// </summary>
    void Emit();

    /// <summary>
    /// エディタ更新
    /// </summary>
    void EditorUpdate();

    /// <summary>
    /// Rail放出開始
    /// </summary>
    void StartRailEmit();

    /// <summary>
    /// デバッグライン設定
    /// </summary>
    void SetEmitLine();

private:
    void InitWithModel(
        const std::string& name,
        const std::string& modelFilePath,
        int32_t maxCount);

    void InitWithPrimitive(
        const std::string& name,
        const PrimitiveType& type,
        int32_t maxCount);

    void ParameterInit();
    void RegisterParams();
    void ApplyParameters();
    void ApplyTexture(const std::string& textureName);
    void AdaptTexture();

    void RailMoveUpdate();
    void UpdateEmitTransform();

    void EmitParameterEditor();
    void EmitterSettingsEditor();
    void ImGuiTextureSelection();
    void ParticlePresetChange();

    void DisplayFileSelection(
        const std::string& header,
        const std::vector<std::string>& filenames,
        int& selectedIndex,
        const std::function<void(const std::string&)>& onApply);

private:
    std::string name_;

    EmitParameters emitParams_;
    EmitterSettings emitterSettings_;
    GroupSettings groupSettings_;

    GlobalParameter* globalParameter_ = nullptr;
    std::unique_ptr<RailManager> railManager_;
    std::unique_ptr<Line3D> debugLine_;
    WorldTransform emitBoxTransform_;

    float currentTime_    = 0.0f;
    bool shouldEmit_      = false;
    bool isStartRailMove_ = false;

    int blendModeIndex_ = 1;

    const std::string folderPath_      = "GPUParticle";
    const std::string textureFilePath_ = "Resources/texture";
    std::string selectedTexturePath_;

    bool isMoveForRail_ = false;
    bool isRailLoop_    = false;
    float moveSpeed_    = 1.0f;

public:
    // Getters
    const std::string& GetName() const { return name_; }
    const EmitParameters& GetEmitParams() const { return emitParams_; }
    const EmitterSettings& GetEmitterSettings() const { return emitterSettings_; }
    const GroupSettings& GetGroupSettings() const { return groupSettings_; }
    bool GetIsMoveForRail() const { return isMoveForRail_; }
    float GetMoveSpeed() const { return moveSpeed_; }

    // Setters
    void SetEmitParams(const EmitParameters& params) { emitParams_ = params; }
    void SetEmitterSettings(const EmitterSettings& settings) { emitterSettings_ = settings; }
    void SetGroupSettings(const GroupSettings& settings) { groupSettings_ = settings; }
    void SetTexture(uint32_t textureHandle);
    void SetEmitterData(const ParticleEmit& emitter);
    void SetParentBasePos(WorldTransform* parent) { emitBoxTransform_.parent_ = parent; }
};