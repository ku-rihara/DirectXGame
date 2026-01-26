#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ShakeData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// シェイクプレイヤー
/// </summary>
namespace KetaEngine {

class ShakePlayer : public BaseEffectPlayer {
public:
    ShakePlayer()           = default;
    ~ShakePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectPlayerからのオーバーライド
    void Init() override;
    void Update(float speedRate = 1.0f) override;
    void Play(const std::string& shakeName, const std::string& categoryName = "Common") override;

private:
    //*---------------------------- private Methods ----------------------------*//

    std::unique_ptr<BaseEffectData> CreateEffectData() override;
    void UpdateTotalShakeOffset();

private:
    //*---------------------------- private Variant ----------------------------*//

    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

public:
    //*----------------------------- getter Methods -----------------------------*//

    const Vector3& GetTotalShakeOffset() const { return totalShakeOffset_; }
};

}; // KetaEngine
