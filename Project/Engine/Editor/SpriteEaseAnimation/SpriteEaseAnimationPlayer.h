#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "SpriteEaseAnimationData.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// スプライトイージングアニメーションプレイヤー
/// </summary>
namespace KetaEngine {

class SpriteEaseAnimationPlayer : public BaseEffectPlayer {
public:
    SpriteEaseAnimationPlayer()           = default;
    ~SpriteEaseAnimationPlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    void Init() override;
    void Update(float speedRate = 1.0f) override;
    void Play(const std::string& animationName, const std::string& categoryName = "Common") override;

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

public:
    //*----------------------------- getter Methods -----------------------------*//

    Vector2 GetCurrentScale() const;
    Vector2 GetCurrentPosition() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentColor() const;
    float GetCurrentAlpha() const;

    SpriteEaseAnimationData* GetAnimationData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};

}; // KetaEngine
