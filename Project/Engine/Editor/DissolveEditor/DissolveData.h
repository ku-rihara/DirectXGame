#pragma once
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// ディゾルブデータクラス
/// </summary>
namespace KetaEngine {

class DissolveData : public BaseEffectData {
public:
    DissolveData()  = default;
    ~DissolveData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="name">ディゾルブ名</param>
    void Init(const std::string& name) override;

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void Update(float speedRate = 1.0f) override;

    void AdjustParam(); //< パラメータ調整
    void Play() override; //< 再生
    void Reset() override; //< リセット

    void LoadData() override; //< データ読み込み
    void SaveData() override; //< データ保存

private:
    void RegisterParams() override; //< パラメータバインド
    void GetParams() override; //< パラメータ取得
    void InitParams() override; //< パラメータ初期化
    void UpdateDissolveValues(); //< ディゾルブ値更新
    void LoadNoiseTextures(); //< ノイズテクスチャ読み込み

private:
    // パラメータ
    float startThreshold_         = 1.0f;
    float endThreshold_           = 0.0f;
    float maxTime_                = 1.0f;
    float offsetTime_             = 0.0f;
    int32_t easeType_             = 0;
    int32_t selectedTextureIndex_ = 0;

    // 状態
    float currentTime_      = 0.0f;
    float totalTime_        = 0.0f;
    float currentThreshold_ = 1.0f;
    bool currentEnable_     = false;

    // イージング
    Easing<float> thresholdEase_;
    float easedThreshold_ = 1.0f;

    // テクスチャ管理
    std::string textureFilePath_ = "Resources/EngineTexture/Noise";
    std::vector<std::string> noiseTextureFiles_;
    std::string currentTexturePath_;

public:
    // getter

    const std::string& GetCurrentTexturePath() const { return currentTexturePath_; }
    float GetCurrentThreshold() const { return currentThreshold_; }
    bool IsDissolveEnabled() const { return currentEnable_; }
  

    // setter
    void SetTextureIndex(int32_t index);
};

}; // KetaEngine