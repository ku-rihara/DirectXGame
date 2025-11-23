#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ObjEaseAnimationData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションプレイヤー
/// </summary>
class ObjEaseAnimationPlayer : public BaseEffectPlayer {
public:
    ObjEaseAnimationPlayer()           = default;
    ~ObjEaseAnimationPlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectPlayerからのオーバーライド
    void Init() override;
    void Update(const float& speedRate = 1.0f) override;

    // 再生
    void PlayInCategory(const std::string& categoryName, const std::string& animationName);

private:

    //*---------------------------- private Variant ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;
    std::string currentCategoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    Vector3 GetCurrentScale() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentTranslation() const;

    ObjEaseAnimationData* GetAnimationData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};