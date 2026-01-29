#pragma once
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

/// <summary>
/// シェイクデータ
/// </summary>
namespace KetaEngine {

class ShakeData : public BaseEffectData {
public:
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
    ShakeData()           = default;
    ~ShakeData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& shakeName, const std::string& categoryName) override;
    void Update(float speedRate = 1.0f) override;
    void Reset() override;
    void Play() override;

    // シェイク固有
    void AdjustParam();

private:
    //*---------------------------- private Methods ----------------------------*//

    // BaseEffectDataからのオーバーライド
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // シェイク値計算
    void UpdateShakeValues();
    void UpdateVector3Shake();
    Vector3 ApplyAxisFlag(const Vector3& shakeValue) const;

private:
    //*---------------------------- private Variant ----------------------------*//

    const std::string baseFolderPath_ = "ShakeEditor/";

    float shakeLength_ = 1.0f;
    float maxTime_     = 1.0f;
    float startTime_   = 1.0f;
    int32_t easeType_  = 0;
    int32_t shakeType_ = 0;
    int32_t axisFlag_  = AXIS_XYZ;

    Vector3 currentShakeOffset_ = Vector3::ZeroVector();

    Easing<float> timeEase_;
    float easedTime_ = 0.0f;

    TimeModeSelector timeModeSelector_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    float GetShakeLength() const { return shakeLength_; }
    float GetMaxTime() const { return maxTime_; }
    Vector3 GetShakeOffset() const { return currentShakeOffset_; }
    AxisFlag GetAxisFlag() const { return static_cast<AxisFlag>(axisFlag_); }

    void SetAxisFlag(const AxisFlag& flag) { axisFlag_ = static_cast<int32_t>(flag); }
};

}; // KetaEngine