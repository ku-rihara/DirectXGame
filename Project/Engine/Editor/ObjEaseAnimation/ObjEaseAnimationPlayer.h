#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ObjEaseAnimationData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// オブジェクトイージングアニメーションプレイヤー
/// </summary>
namespace KetaEngine {

class ObjEaseAnimationPlayer : public BaseEffectPlayer {
public:
    ObjEaseAnimationPlayer()           = default;
    ~ObjEaseAnimationPlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectPlayerからのオーバーライド
    void Init() override;
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& animationName);

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string currentCategoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    Vector3 GetCurrentScale() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentTranslation() const;

    ObjEaseAnimationData* GetAnimationData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};

}; // KetaEngine
