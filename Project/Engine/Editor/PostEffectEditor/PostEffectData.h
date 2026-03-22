#pragma once
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "Easing/Easing.h"
#include "PostEffect/PostEffectRenderer.h"
#include "Vector2.h"
#include "Vector3.h"
#include <string>

/// <summary>
/// ポストエフェクトデータ
/// </summary>
namespace KetaEngine {

class PostEffectData : public BaseEffectData {
public:
    PostEffectData()  = default;
    ~PostEffectData() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& name, const std::string& categoryName) override;
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // ImGuiパラメータ調整
    void AdjustParam();

    // レンダラーへパラメータを適用してエフェクトを有効化
    void ApplyToRenderer();

    // モードの主要パラメータを設定（イージング用）
    void SetMainParam(float value);

private:
    //*---------------------------- private Methods ----------------------------*//

    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

private:
    //*---------------------------- private Variant ----------------------------*//

    // 共通パラメータ
    int32_t postEffectModeIndex_ = 0;
    float startTime_             = 0.0f; //< 再生開始までの遅延時間

    // GaussianFilter 固有
    float gaussSigma_ = 1.0f;

    // RadialBlur 固有
    Vector2 radialCenter_  = {0.5f, 0.5f};
    float radialBlurWidth_ = 0.01f;

    // Dissolve 固有
    float dissolveThreshold_ = 0.5f;
    Vector3 dissolveColor_   = {1.0f, 0.0f, 0.0f};

    // Outline 固有
    float outlineWeightRate_ = 0.3f;

    // LuminanceOutline 固有
    float luminanceWeightRate_ = 0.3f;

    // イージングパラメータ
    float durationTime_ = 0.0f; //< エフェクト持続時間 (0=瞬時適用)
    float paramStart_   = 0.0f; //< 主要パラメータのイージング開始値
    float paramEnd_     = 0.0f; //< 主要パラメータのイージング終了値
    int32_t easeType_   = 0;    //< EasingType にキャストして使用

    // イージングオブジェクト
    Easing<float> paramEase_;
    float easedParam_ = 0.0f;

    // 内部タイマー（startTime 待機用）
    float currentTime_ = 0.0f;

public:
    //*----------------------------- getter Methods -----------------------------*//

    PostEffectMode GetPostEffectMode() const { return static_cast<PostEffectMode>(postEffectModeIndex_); }
    float GetStartTime() const { return startTime_; }
};

} // namespace KetaEngine
