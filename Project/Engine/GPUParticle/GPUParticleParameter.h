#pragma once

#include "3d/ViewProjection.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "MinMax.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

/// <summary>
/// GPUパーティクルパラメータクラス
/// </summary>
class GPUParticleParameter {
public:
    struct EmitParameters {
        // Scale parameters
        Vector3 scaleMin = {1.0f, 1.0f, 1.0f};
        Vector3 scaleMax = {1.0f, 1.0f, 1.0f};

        // Rotation parameters (degree)
        Vector3 rotationMin = {0.0f, 0.0f, 0.0f};
        Vector3 rotationMax = {0.0f, 0.0f, 0.0f};

        // Rotation speed parameters (degree/sec)
        Vector3 rotationSpeedMin = {0.0f, 0.0f, 0.0f};
        Vector3 rotationSpeedMax = {0.0f, 0.0f, 0.0f};

        // Position offset parameters
        Vector3 translateMin = {0.0f, 0.0f, 0.0f};
        Vector3 translateMax = {0.0f, 0.0f, 0.0f};

        // Velocity parameters
        Vector3 velocityMin = {0.0f, 0.0f, 0.0f};
        Vector3 velocityMax = {0.0f, 0.0f, 0.0f};

        // Color parameters
        Vector4 colorMin = {1.0f, 1.0f, 1.0f, 1.0f};
        Vector4 colorMax = {1.0f, 1.0f, 1.0f, 1.0f};

        // LifeTime parameters
        float lifeTimeMin = 1.0f;
        float lifeTimeMax = 3.0f;
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
    GPUParticleParameter()          = default;
    virtual ~GPUParticleParameter() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="name">パーティクル名</param>
    virtual void Init(const std::string& name);

    /// <summary>
    /// パラメータの初期化
    /// </summary>
    void ParameterInit();

    /// <summary>
    /// パラメータの登録
    /// </summary>
    void RegisterParams();

    /// <summary>
    /// エディタ更新
    /// </summary>
    virtual void EditorUpdate();

    /// <summary>
    /// パラメータの適用
    /// </summary>
    void ApplyParameters();

    /// <summary>
    /// テクスチャの適用
    /// </summary>
    void ApplyTexture(const std::string& textureName);

    /// <summary>
    /// テクスチャの自動適用
    /// </summary>
    void AdaptTexture();

protected:
    /// <summary>
    /// Emitパラメータエディタ
    /// </summary>
    void EmitParameterEditor();

    /// <summary>
    /// エミッター設定エディタ
    /// </summary>
    void EmitterSettingsEditor();

    /// <summary>
    /// テクスチャ選択ImGui
    /// </summary>
    void ImGuiTextureSelection();

    /// <summary>
    /// ファイル選択表示
    /// </summary>
    void DisplayFileSelection(
        const std::string& header,
        const std::vector<std::string>& filenames,
        int& selectedIndex,
        const std::function<void(const std::string&)>& onApply);

    /// <summary>
    /// パーティクルプリセット変更
    /// </summary>
    void ParticlePresetChange();

protected:
    std::string particleName_;
    EmitParameters emitParams_;
    EmitterSettings emitterSettings_;
    GroupSettings groupSettings_;

    GlobalParameter* globalParameter_ = nullptr;
    std::unique_ptr<RailManager> railManager_;

    // UI関連
    int blendModeIndex_ = 1; // Default: Add

    // ファイルパス
    const std::string folderPath_      = "GPUParticle";
    const std::string textureFilePath_ = "Resources/texture";
    std::string selectedTexturePath_;

    // Rail関連
    bool isMoveForRail_ = false;
    bool isRailLoop_    = false;
    float moveSpeed_    = 1.0f;

public:
    // Getters
    const std::string& GetParticleName() const { return particleName_; }
    const EmitParameters& GetEmitParams() const { return emitParams_; }
    const EmitterSettings& GetEmitterSettings() const { return emitterSettings_; }
    const GroupSettings& GetGroupSettings() const { return groupSettings_; }

    // Setters
    void SetEmitParams(const EmitParameters& params) { emitParams_ = params; }
    void SetEmitterSettings(const EmitterSettings& settings) { emitterSettings_ = settings; }
    void SetGroupSettings(const GroupSettings& settings) { groupSettings_ = settings; }
};