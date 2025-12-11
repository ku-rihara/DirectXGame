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
    void Update(float speedRate = 1.0f) override;

    void Play(const std::string& effectName) override;
    void PlayInCategory(const std::string& categoryName, const std::string& animationName);

    // オフセット値のリセット
    void ResetOffsets();

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Methods ----------------------------*//
    void SaveCurrentOffsets();

private:
    //*---------------------------- private Variant ----------------------------*//
    std::string currentCategoryName_;

    // 中断時の値を保持
    Vector3 currentOffsetScale_       = Vector3::OneVector();
    Vector3 currentOffsetRotation_    = Vector3::ZeroVector();
    Vector3 currentOffsetTranslation_ = Vector3::ZeroVector();

public:
    //*----------------------------- getter Methods -----------------------------*//

    Vector3 GetCurrentScale() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentTranslation() const;

    ObjEaseAnimationData* GetAnimationData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
};