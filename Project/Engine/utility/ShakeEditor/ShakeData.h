#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

/// <summary>
/// シェイクデータ
/// </summary>
class ShakeData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

    enum class ShakeType {
        NORMAL,
        WAVE
    };

    enum AxisFlag : uint32_t {
        AXIS_NONE = 0,
        AXIS_X    = 1 << 0,
        AXIS_Y    = 1 << 1,
        AXIS_Z    = 1 << 2,
        AXIS_XY   = AXIS_X | AXIS_Y,
        AXIS_XZ   = AXIS_X | AXIS_Z,
        AXIS_YZ   = AXIS_Y | AXIS_Z,
        AXIS_XYZ  = AXIS_X | AXIS_Y | AXIS_Z
    };

public:
    ShakeData()  = default;
    ~ShakeData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="shakeName">シェイク名</param>
    void Init(const std::string& shakeName, const bool& bindSkip = false);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    void AdjustParam(); //< パラメータ調整
    void Play();        //< 再生
    void Stop();        //< 停止
    void Reset();       //< リセット
    void LoadData();    //< データロード
    void SaveData();    //< データセーブ
    bool IsPlaying() const; //< 再生中か
    bool IsFinished() const; //< 終了したか

private:
    void BindParams(); //< パラメータのバインド
    void UpdateShakeValues(); //< シェイク値の計算
    void UpdateVector3Shake(); //< Vector3シェイクの更新

    /// <summary>
    /// イージングタイプセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象変数</param>
    void EasingTypeSelector(const char* label, int32_t& target);

    /// <summary>
    /// 軸フラグの適用
    /// </summary>
    /// <param name="shakeValue">シェイク値</param>
    /// <returns>軸フラグ適用後の値</returns>
    Vector3 ApplyAxisFlag(const Vector3& shakeValue) const;

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "ShakeEditor";

    float shakeLength_ = 1.0f;
    float maxTime_     = 1.0f;
    float startTime_   = 1.0f;
    int32_t easeType_  = 0;
    int32_t shakeType_ = 0;
    int32_t axisFlag_  = AXIS_XYZ;

    PlayState playState_ = PlayState::STOPPED;

    Vector3 currentShakeOffset_ = {0.0f, 0.0f, 0.0f};

    Easing<float> timeEase_;
    float easedTime_ = 0.0f;

    bool showControls_ = true;

public:
    std::string GetGroupName() const { return groupName_; }
    float GetShakeLength() const { return shakeLength_; }
    float GetMaxTime() const { return maxTime_; }
    Vector3 GetShakeOffset() const { return currentShakeOffset_; }
    AxisFlag GetAxisFlag() const { return static_cast<AxisFlag>(axisFlag_); }

    void SetAxisFlag(AxisFlag flag) { axisFlag_ = static_cast<int32_t>(flag); }
};