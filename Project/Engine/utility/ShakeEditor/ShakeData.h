#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

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

    // 軸設定用のフラグ
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

    /// 初期化、更新、調節
    void Init(const std::string& shakeName);
    void Update(float deltaTime);
    void AdjustParam();

    /// 再生制御
    void Play();
    void Stop();
    void Reset();

    /// 　Load,Save
    void LoadData();
    void SaveData();

    /// 状態取得
    bool IsPlaying() const;
    bool IsFinished() const;

private:
    /// Editor
    void BindParams();
    void EasingTypeSelector(const char* label, int32_t& target);

    /// シェイク値の計算
    void UpdateShakeValues();

    // 適応
    Vector3 ApplyAxisFlag(const Vector3& shakeValue) const;
    void UpdateVector3Shake();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "ShakeEditor";

    // シェイクパラメータ
    float shakeLength_ = 1.0f;
    float maxTime_     = 1.0f;
    float startTime_   = 1.0f;
    int32_t easeType_  = 0;
    int32_t shakeType_ = 0;
    int32_t axisFlag_  = AXIS_XYZ;

    // 再生状態
    PlayState playState_ = PlayState::STOPPED;

    // 現在のシェイク値
    Vector3 currentShakeOffset_ = {0.0f, 0.0f, 0.0f};

    // イージング用
    Easing<float> timeEase_;
    float easedTime_ = 0.0f;

    // UI表示
    bool showControls_ = true;

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    std::string GetGroupName() const { return groupName_; }
    float GetShakeLength() const { return shakeLength_; }
    float GetMaxTime() const { return maxTime_; }
    Vector3 GetShakeOffset() const { return currentShakeOffset_; }
    AxisFlag GetAxisFlag() const { return static_cast<AxisFlag>(axisFlag_); }
    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetAxisFlag(AxisFlag flag) { axisFlag_ = static_cast<int32_t>(flag); }

};