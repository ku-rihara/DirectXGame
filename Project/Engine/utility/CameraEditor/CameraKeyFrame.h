#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// カメラキーフレームクラス
/// </summary>
class CameraKeyFrame {
public:
    struct KeyFrameParam {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 rotation = {0.0f, 0.0f, 0.0f};
        float fov        = 45.0f;
    };

    enum class TimeMode {
        DELTA_TIME      = 0,
        DELTA_TIME_RATE = 1
    };

public:
    CameraKeyFrame()  = default;
    ~CameraKeyFrame() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="cameraAnimationName">カメラアニメーション名</param>
    /// <param name="keyNumber">キー番号</param>
    void Init(const std::string& cameraAnimationName, const int32_t& keyNumber);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void Update(const float& speedRate);

    /// <summary>
    /// タイムモードセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象</param>
    void TimeModeSelector(const char* label, int32_t& target);

    void AdjustParam();       //< パラメータ調整
    void BindParams();        //< パラメータバインド
    void AdaptEaseParam();    //< イージングパラメータ適用
    void AdaptValueSetting(); //< 値設定適用
    void Reset();             //< リセット
    bool IsFinished() const;  //< 終了判定

    void LoadData(); //< データ読み込み
    void SaveData(); //< データ保存

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderName_ = "CameraAnimation/KeyFrames";
    int32_t currentKeyFrameIndex  = -1;

    float timePoint_ = 0.0f;
    KeyFrameParam keyFrameParam_;
    KeyFrameParam currentKeyFrameParam_;

    int32_t positionEaseType_ = 0;
    int32_t rotationEaseType_ = 0;
    int32_t fovEaseType_      = 0;

    int32_t timeMode_ = static_cast<int32_t>(TimeMode::DELTA_TIME_RATE);

    Easing<Vector3> positionEase_;
    Easing<Vector3> rotationEase_;
    Easing<float> fovEase_;

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
    const float& GetTimePoint() const { return timePoint_; }
    const Vector3& GetPosition() const { return currentKeyFrameParam_.position; }
    const Vector3& GetRotation() const { return currentKeyFrameParam_.rotation; }
    const float& GetFov() const { return currentKeyFrameParam_.fov; }
    const Vector3& GetEditPosition() const { return keyFrameParam_.position; }
    const Vector3& GetEditRotation() const { return keyFrameParam_.rotation; }
    const float& GetEditFov() const { return keyFrameParam_.fov; }
    const TimeMode& GetTimeMode() const { return static_cast<TimeMode>(timeMode_); }

    void SetTimePoint(const float& timePoint) { timePoint_ = timePoint; }
    void SetStartEasing(const Vector3& pos, const Vector3& rotate, const float& fov);
    void SetTimeMode(const TimeMode& mode) { timeMode_ = static_cast<int32_t>(mode); }
};