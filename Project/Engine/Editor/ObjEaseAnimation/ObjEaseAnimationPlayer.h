#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "ObjEaseAnimationData.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

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
    void Play(const std::string& animationName, const std::string& categoryName = "Common") override;

protected:
    //*---------------------------- protected Methods ----------------------------*//
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    //*---------------------------- private Variant ----------------------------*//
    std::unordered_map<std::string, std::function<void()>> namedLoopEndCallbacks_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    Vector3 GetCurrentScale() const;
    Vector3 GetCurrentRotation() const;
    Vector3 GetCurrentTranslation() const;

    Vector3 GetCurrentScaleAnchor() const;
    Vector3 GetCurrentRotationAnchor() const;
    Vector3 GetCurrentTranslationAnchor() const;

    ObjEaseAnimationData* GetAnimationData();
    const std::string& GetCurrentCategoryName() const { return currentCategoryName_; }
    bool IsLookingAtDirection() const;
    Vector3 GetMovementDirection() const;
    bool IsTranslationReturning() const;

    //*----------------------------- setter Methods -----------------------------*//
    void SetLoop(bool isLoop);

    /// アニメーション名単位でループ終了コールバックを登録
    void SetLoopEndCallback(const std::string& animationName, std::function<void()> callback);

    /// 現在再生中のアニメーションにコールバックを直接セット
    void SetLoopEndCallback(const std::function<void()>& callback);

    void SetPreAnimationOffsetsToOriginalValues();
};

}; // KetaEngine
