#pragma once
#include "DissolveData.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include <memory>
#include <string>

/// <summary>
/// ディゾルブ再生クラス
/// </summary>
namespace KetaEngine {
class ModelMaterial;
class DissolvePlayer : public BaseEffectPlayer {
public:
    DissolvePlayer()           = default;
    ~DissolvePlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    /// <summary>
    /// ディゾルブ再生
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    void Play(const std::string& dissolveName, const std::string& category = "Common") override;

    /// マテリアルに適用
    /// </summary>
    /// <param name="material">適用先のマテリアル</param>
    void ApplyToMaterial(ModelMaterial& material);

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    DissolveData* GetDissolveData();
};

}; // KetaEngine