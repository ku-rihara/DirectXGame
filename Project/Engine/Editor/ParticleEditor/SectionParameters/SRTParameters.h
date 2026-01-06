#pragma once
#include "../Parameter/ParticleCommonParameters.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MinMax.h"
#include "Vector3.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// Scale, Rotate, Translateのパラメータ
/// </summary>
class SRTParameters {
public:
    using ScaleEaseParam = ParticleCommon::ScaleEaseParam;
    using EasingParamV3  = ParticleCommon::EasingParamV3;

    SRTParameters()  = default;
    ~SRTParameters() = default;

    void RegisterParams(GlobalParameter* globalParam, const std::string& groupName);
    void AdaptParameters(GlobalParameter* globalParam, const std::string& groupName);
    void AdjustParam();

private:
    void ScaleParamEditor();
    void RotateParamEditor();
    void PositionParamEditor();
    void VelocityParamEditor();

    // Scale
    V3MinMax scaleDistV3_;
    FMinMax scaleDist_;
    ScaleEaseParam scaleEaseParam_;
    bool isScalerScale_ = true;

    // Rotate
    Vector3 baseRotate_{0.0f, 0.0f, 0.0f};
    V3MinMax rotateDist_;
    V3MinMax rotateSpeedDist_;
    EasingParamV3 rotateEaseParam_;
    bool isRotateForDirection_ = false;

    // Translate(Position)
    Vector3 emitPos_{0.0f, 0.0f, 0.0f};
    V3MinMax positionDist_;
    EasingParamV3 translateEaseParam_;

    // Velocity
    V3MinMax directionDist_;
    V3MinMax velocityDistV3_;
    FMinMax speedDist_;
    float gravity_        = 0.0f;
    bool isFloatVelocity_ = false;

public:
    // Scale Getters
    const V3MinMax& GetScaleDistV3() const { return scaleDistV3_; }
    const FMinMax& GetScaleDist() const { return scaleDist_; }
    const ScaleEaseParam& GetScaleEaseParam() const { return scaleEaseParam_; }
    bool IsScalerScale() const { return isScalerScale_; }

    // Rotate Getters
    const Vector3& GetBaseRotate() const { return baseRotate_; }
    const V3MinMax& GetRotateDist() const { return rotateDist_; }
    const V3MinMax& GetRotateSpeedDist() const { return rotateSpeedDist_; }
    const EasingParamV3& GetRotateEaseParam() const { return rotateEaseParam_; }
    bool IsRotateForDirection() const { return isRotateForDirection_; }

    // Translate(Position) Getters
    const Vector3& GetEmitPos() const { return emitPos_; }
    const V3MinMax& GetPositionDist() const { return positionDist_; }
    const EasingParamV3& GetTranslateEaseParam() const { return translateEaseParam_; }

    // Velocity Getters
    const V3MinMax& GetDirectionDist() const { return directionDist_; }
    const V3MinMax& GetVelocityDistV3() const { return velocityDistV3_; }
    const FMinMax& GetSpeedDist() const { return speedDist_; }
    float GetGravity() const { return gravity_; }
    bool IsFloatVelocity() const { return isFloatVelocity_; }

    // Setters
    void SetEmitPos(const Vector3& pos) { emitPos_ = pos; }
    void SetBaseRotate(const Vector3& rotate) { baseRotate_ = rotate; }
    void SetGravity(float gravity) { gravity_ = gravity; }
    void SetIsScalerScale(bool flag) { isScalerScale_ = flag; }
};

} // namespace KetaEngine