#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>

/// <summary>
/// ディゾルブデータクラス
/// </summary>
class DissolveData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

public:
    DissolveData()  = default;
    ~DissolveData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    void Init(const std::string& dissolveName);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

    void AdjustParam(); //< パラメータ調整
    void Play();        //< 再生
    void Stop();        //< 停止
    void Reset();       //< リセット

    void LoadData(); //< データ読み込み
    void SaveData(); //< データ保存

    bool IsPlaying() const; //< 再生中判定
    bool IsFinished() const; //< 終了判定

private:
    void BindParams();           //< パラメータバインド
    void UpdateDissolveValues(); //< ディゾルブ値更新

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "DissolveEditor";
    float startThreshold_   = 1.0f;
    float maxTime_          = 1.0f;
    float endThreshold_;
    float offsetTime_;
    int32_t easeType_       = 0;
    PlayState playState_    = PlayState::STOPPED;
    float currentTime_      = 0.0f;
    float totalTime_        = 0.0f;
    float currentThreshold_ = 1.0f;
    bool currentEnable_     = false;
    Easing<float> thresholdEase_;
    float easedThreshold_ = 1.0f;
    bool showControls_    = true;

public:

    // getter
    std::string GetGroupName() const { return groupName_; }
    float GetCurrentThreshold() const { return currentThreshold_; }
    bool IsDissolveEnabled() const { return currentEnable_; }
    float GetStartThreshold() const { return startThreshold_; }
    float GetEndThreshold() const { return endThreshold_; }
    float GetMaxTime() const { return maxTime_; }
    float GetOffsetTime() const { return offsetTime_; }

    // setter
    void SetStartThreshold(float threshold) { startThreshold_ = threshold; }
    void SetEndThreshold(float threshold) { endThreshold_ = threshold; }
    void SetMaxTime(float time) { maxTime_ = time; }
    void SetOffsetTime(float time) { offsetTime_ = time; }
    void SetEaseType(int32_t type) { easeType_ = type; }
};