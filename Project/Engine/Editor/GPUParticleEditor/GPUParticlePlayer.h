#pragma once
#include "3d/WorldTransform.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "GPUParticleData.h"
#include <memory>
#include <string>

/// <summary>
/// GPUパーティクルプレイヤー
/// </summary>
class GPUParticlePlayer : public BaseEffectPlayer {
public:
    GPUParticlePlayer()           = default;
    ~GPUParticlePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& particleName);

    // 描画
    void Draw();

    // エミット制御
    void EmitOnce(); // 一度だけエミット
    void StartEmit(); // 連続エミット開始
    void StopEmit(); // 連続エミット停止

    // トランスフォーム設定
    void SetEmitPosition(const Vector3& position);
    void SetParentTransform(WorldTransform* parent);

    // エミッター設定
    void SetEmitterCount(uint32_t count);
  
protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string currentCategoryName_;
    bool isEmitting_     = false;
    bool shouldEmitOnce_ = false;

public:
    //*----------------------------- getter Methods -----------------------------*//
    GPUParticleData* GetParticleData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
    bool IsEmitting() const { return isEmitting_; }
};