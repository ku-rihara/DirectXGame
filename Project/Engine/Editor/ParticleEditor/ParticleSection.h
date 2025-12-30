#pragma once
#include "3d/WorldTransform.h"
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/RailEditor/RailManager.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "Line3D/Line3D.h"
#include "ParticleParameter.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Primitive/IPrimitive.h"
#include "utility/FileSelector/FileSelector.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
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

    /// <summary>
    /// 初期化
    /// </summary>
    void Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex);

    /// <summary>
    /// 更新
    /// </summary>
    void Update(float speedRate = 1.0f);

    /// <summary>
    /// リセット
    /// </summary>
    void Reset();

    /// <summary>
    /// このセクションが終了したかどうか
    /// </summary>
    bool IsFinished() const;

    //*----------------------------- Playback Control -----------------------------*//

    /// <summary>
    /// 再生開始
    /// </summary>
    void Play();

    /// <summary>
    /// 一時停止
    /// </summary>
    void Pause();

    /// <summary>
    /// 停止
    /// </summary>
    void Stop();

    /// <summary>
    /// 待機状態で開始
    /// </summary>
    void StartWaiting();

    //*----------------------------- Manual Emit -----------------------------*//

    /// <summary>
    /// パーティクルを放出
    /// </summary>
    void EmitManual();

    //*----------------------------- Editor Methods -----------------------------*//

    void LoadData();
    void SaveData();
    void AdjustParam();

    //*----------------------------- Setup Methods -----------------------------*//

    /// <summary>
    /// モデルパーティクルの作成
    /// </summary>
    void CreateModelParticle(const std::string& modelFilePath, int32_t maxnum);

    /// <summary>
    /// プリミティブパーティクルの作成
    /// </summary>
    void CreatePrimitiveParticle(PrimitiveType primitiveType, int32_t maxnum);

    /// <summary>
    /// プリミティブタイプの変更
    /// </summary>
    void ChangePrimitive(const PrimitiveType& primitiveType);

    /// <summary>
    /// テクスチャの設定
    /// </summary>
    void SetTextureHandle(uint32_t handle);

      void AdaptTexture();

private:
    //*---------------------------- Internal Methods ----------------------------*//

    void RegisterAdditionalParams();
    void GetAdditionalParams();
    void ApplyTexture(const std::string& textureName);
  
    void AdaptEaseSettings();
    void AdaptRailSettings();

    void UpdateEmitTransform();
    void SetEmitLine();

    void StartPlay();
    void UpdateWaiting(float deltaTime);

    void ImGuiTextureSelection();

    /// <summary>
    /// Emit処理
    /// </summary>
    void EmitInternal();

private:
    //*---------------------------- Member Variables ----------------------------*//

    std::string sectionName_;
    std::string particleName_;
    std::string categoryName_;
    std::string groupName_;
    int32_t sectionIndex_ = -1;

    // パラメータクラスを使用
    std::unique_ptr<ParticleParameter> parameterHandler_;

    // スケールイージング用
    Easing<Vector3> scaleEasing_;
    bool useScaleEasing_ = false;

    GlobalParameter* globalParameter_ = nullptr;
    std::unique_ptr<RailPlayer> railPlayer_;
    std::unique_ptr<Line3D> debugLine_;
    WorldTransform emitBoxTransform_;

    float currentTime_ = 0.0f; // Emit間隔タイマー
    float elapsedTime_ = 0.0f; // 待機時間タイマー
    float startTime_   = 0.0f; // 開始待機時間

    const std::string baseFolderPath_  = "Particle/";
    const std::string textureFilePath_ = "Resources/texture";
    const std::string modelBasePath_   = "Resources/Model/";
    std::string folderPath_;

    // Rail関連
    bool useRail_ = false;
    std::string railFileName_;
    FileSelector railFileSelector_;

    // Model関連
    FileSelector modelFileSelector_;

    PlayState playState_ = PlayState::STOPPED;
    TimeModeSelector timeModeSelector_;

public:
    //*----------------------------- Getters -----------------------------*//

    const std::string& GetSectionName() const { return sectionName_; }
    const std::string& GetGroupName() const { return groupName_; }
    ParticleParameter::Parameters& GetParameters() { return parameterHandler_->parameters_; }
    ParticleParameter::GroupParamaters& GetGroupParameters() { return parameterHandler_->groupParamaters_; }
    int32_t GetParticleCount() const { return parameterHandler_->particleCount_; }
    float GetIntervalTime() const { return parameterHandler_->intervalTime_; }
    int32_t GetMaxParticleNum() const { return parameterHandler_->maxParticleNum_; }
    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }
    PlayState GetPlayState() const { return playState_; }
    bool IsPlaying() const { return playState_ == PlayState::PLAYING; }
    ParticleParameter* GetParticleParameter() { return parameterHandler_.get(); }
    const ParticleParameter* GetParticleParameter() const { return parameterHandler_.get(); }

    //*----------------------------- Setters -----------------------------*//

    void SetTexture(uint32_t textureHandle);
    void SetParentBasePos(WorldTransform* parent) { emitBoxTransform_.parent_ = parent; }
};

}; // KetaEngine