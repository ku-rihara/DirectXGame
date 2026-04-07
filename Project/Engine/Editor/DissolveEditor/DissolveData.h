#pragma once
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "utility/FileSelector/FileSelector.h"
#include <cstdint>
#include <string>

/// <summary>
/// ディゾルブデータクラス
/// </summary>
namespace KetaEngine {

class DissolveData : public BaseEffectData {
public:
    DissolveData()  = default;
    ~DissolveData() = default;

    /// <summary>
    /// カテゴリー対応初期化
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    /// <param name="categoryName">カテゴリー名</param>
    void Init(const std::string& dissolveName, const std::string& categoryName) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void Update(float speedRate = 1.0f) override;

    void AdjustParam(); //< パラメータ調整
    void Play() override; //< 再生
    void Reset() override; //< リセット
private:
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;
    void UpdateDissolveValues();

private:
    // パラメータ
    float startThreshold_ = 1.0f;
    float endThreshold_   = 0.0f;
    float maxTime_        = 1.0f;
    float offsetTime_     = 0.0f;
    int32_t easeType_     = 0;

    // 状態
    float currentTime_      = 0.0f;
    float totalTime_        = 0.0f;
    float currentThreshold_ = 1.0f;
    bool currentEnable_     = false;

    // イージング
    Easing<float> thresholdEase_;
    float easedThreshold_ = 1.0f;

    // テクスチャ管理
    const std::string baseFolderPath_  = "DissolveEditor/";
    const std::string textureFilePath_ = "Resources/EngineTexture/Noise";
    std::string currentTexturePath_;
    FileSelector textureSelector_;

public:
    // getter
    const std::string& GetCurrentTexturePath() const { return currentTexturePath_; }
    float GetCurrentThreshold() const { return currentThreshold_; }
    bool IsDissolveEnabled() const { return currentEnable_; }
    float GetStartThreshold() const { return startThreshold_; }
    float GetEndThreshold() const { return endThreshold_; }
    float GetMaxTime() const { return maxTime_; }
    float GetOffsetTime() const { return offsetTime_; }
    int32_t GetEaseType() const { return easeType_; }
};

}; // KetaEngine