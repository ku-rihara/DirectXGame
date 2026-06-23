#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "EasingData.h"
#include <memory>
#include <string>

/// <summary>
/// イージングプレイヤー
/// </summary>
namespace KetaEngine {

template <typename T>
class EasingPlayer : public BaseEffectPlayer {
public:
    EasingPlayer()           = default;
    ~EasingPlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;
    void Update(float speedRate = 1.0f) override;
    void Play(const std::string& easingName, const std::string& categoryName = "Common") override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<BaseEffectData> CreateEffectData() override;

public:
    //*----------------------------- getter Methods -----------------------------*//

    const T& GetValue() const;
};

}; // KetaEngine
