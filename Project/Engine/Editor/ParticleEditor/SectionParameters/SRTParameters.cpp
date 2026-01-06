#include "SRTParameters.h"
#include <imgui.h>

using namespace KetaEngine;


void SRTParameters::RegisterParams(GlobalParameter* globalParam, const std::string& groupName) {
    // Scale
    globalParam->Regist(groupName, "Scale Max", &scaleDist_.max);
    globalParam->Regist(groupName, "Scale Min", &scaleDist_.min);
    globalParam->Regist(groupName, "ScaleV3 Max", &scaleDistV3_.max);
    globalParam->Regist(groupName, "ScaleV3 Min", &scaleDistV3_.min);
    globalParam->Regist(groupName, "isScalerScale", &isScalerScale_);

    // Rotate
    globalParam->Regist(groupName, "Rotate Base", &baseRotate_);
    globalParam->Regist(groupName, "Rotate Max", &rotateDist_.max);
    globalParam->Regist(groupName, "Rotate Min", &rotateDist_.min);
    globalParam->Regist(groupName, "RotateSpeed Max", &rotateSpeedDist_.max);
    globalParam->Regist(groupName, "RotateSpeed Min", &rotateSpeedDist_.min);
    globalParam->Regist(groupName, "isRotateForDirection", &isRotateForDirection_);

    // Position
    globalParam->Regist(groupName, "Position Base", &emitPos_);
    globalParam->Regist(groupName, "Position Max", &positionDist_.max);
    globalParam->Regist(groupName, "Position Min", &positionDist_.min);

    // Velocity
    globalParam->Regist(groupName, "Speed Max", &speedDist_.max);
    globalParam->Regist(groupName, "Speed Min", &speedDist_.min);
    globalParam->Regist(groupName, "SpeedV3 Max", &velocityDistV3_.max);
    globalParam->Regist(groupName, "SpeedV3 Min", &velocityDistV3_.min);
    globalParam->Regist(groupName, "Direction Max", &directionDist_.max);
    globalParam->Regist(groupName, "Direction Min", &directionDist_.min);
    globalParam->Regist(groupName, "isFloatVelocity", &isFloatVelocity_);
    globalParam->Regist(groupName, "Gravity", &gravity_);

    // Scale EaseParam
    globalParam->Regist(groupName, "scaleEaseParam.isScaleEase", &scaleEaseParam_.baseParam.isEase);
    globalParam->Regist(groupName, "scaleEaseParam.maxTime", &scaleEaseParam_.baseParam.maxTime);
    globalParam->Regist(groupName, "scaleEaseParam.easeTypeInt", &scaleEaseParam_.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.max", &scaleEaseParam_.endValueF.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.min", &scaleEaseParam_.endValueF.min);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.max", &scaleEaseParam_.endValueV3.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.min", &scaleEaseParam_.endValueV3.min);
    globalParam->Regist(groupName, "scaleEaseParam.backRatio", &scaleEaseParam_.baseParam.backRatio);

    // Translate EaseParam
    globalParam->Regist(groupName, "TranslateEaseParam.isScaleEase", &translateEaseParam_.baseParam.isEase);
    globalParam->Regist(groupName, "TranslateEaseParam.maxTime", &translateEaseParam_.baseParam.maxTime);
    globalParam->Regist(groupName, "TranslateEaseParam.easeTypeInt", &translateEaseParam_.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "TranslateEaseParam.endValueV3.max", &translateEaseParam_.endValue.max);
    globalParam->Regist(groupName, "TranslateEaseParam.endValueV3.min", &translateEaseParam_.endValue.min);
    globalParam->Regist(groupName, "TranslateEaseParam.backRatio", &translateEaseParam_.baseParam.backRatio);

    // Rotate EaseParam
    globalParam->Regist(groupName, "RotateEaseParam.isScaleEase", &rotateEaseParam_.baseParam.isEase);
    globalParam->Regist(groupName, "RotateEaseParam.maxTime", &rotateEaseParam_.baseParam.maxTime);
    globalParam->Regist(groupName, "RotateEaseParam.easeTypeInt", &rotateEaseParam_.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "RotateEaseParam.endValueV3.max", &rotateEaseParam_.endValue.max);
    globalParam->Regist(groupName, "RotateEaseParam.endValueV3.min", &rotateEaseParam_.endValue.min);
    globalParam->Regist(groupName, "RotateEaseParam.backRatio", &rotateEaseParam_.baseParam.backRatio);
}

void SRTParameters::AdaptParameters(GlobalParameter* globalParam, const std::string& groupName) {
    // Scale
    scaleDist_.max   = globalParam->GetValue<float>(groupName, "Scale Max");
    scaleDist_.min   = globalParam->GetValue<float>(groupName, "Scale Min");
    scaleDistV3_.max = globalParam->GetValue<Vector3>(groupName, "ScaleV3 Max");
    scaleDistV3_.min = globalParam->GetValue<Vector3>(groupName, "ScaleV3 Min");
    isScalerScale_   = globalParam->GetValue<bool>(groupName, "isScalerScale");

    // Rotate
    baseRotate_           = globalParam->GetValue<Vector3>(groupName, "Rotate Base");
    rotateDist_.max       = globalParam->GetValue<Vector3>(groupName, "Rotate Max");
    rotateDist_.min       = globalParam->GetValue<Vector3>(groupName, "Rotate Min");
    rotateSpeedDist_.max  = globalParam->GetValue<Vector3>(groupName, "RotateSpeed Max");
    rotateSpeedDist_.min  = globalParam->GetValue<Vector3>(groupName, "RotateSpeed Min");
    isRotateForDirection_ = globalParam->GetValue<bool>(groupName, "isRotateForDirection");

    // Position
    emitPos_          = globalParam->GetValue<Vector3>(groupName, "Position Base");
    positionDist_.max = globalParam->GetValue<Vector3>(groupName, "Position Max");
    positionDist_.min = globalParam->GetValue<Vector3>(groupName, "Position Min");

    // Velocity
    speedDist_.max      = globalParam->GetValue<float>(groupName, "Speed Max");
    speedDist_.min      = globalParam->GetValue<float>(groupName, "Speed Min");
    velocityDistV3_.max = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Max");
    velocityDistV3_.min = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Min");
    directionDist_.max  = globalParam->GetValue<Vector3>(groupName, "Direction Max");
    directionDist_.min  = globalParam->GetValue<Vector3>(groupName, "Direction Min");
    isFloatVelocity_    = globalParam->GetValue<bool>(groupName, "isFloatVelocity");
    gravity_            = globalParam->GetValue<float>(groupName, "Gravity");

    // Scale EaseParam
    scaleEaseParam_.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "scaleEaseParam.isScaleEase");
    scaleEaseParam_.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "scaleEaseParam.maxTime");
    scaleEaseParam_.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "scaleEaseParam.easeTypeInt");
    scaleEaseParam_.endValueF.max         = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.max");
    scaleEaseParam_.endValueF.min         = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.min");
    scaleEaseParam_.endValueV3.max        = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.max");
    scaleEaseParam_.endValueV3.min        = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.min");
    scaleEaseParam_.baseParam.backRatio   = globalParam->GetValue<float>(groupName, "scaleEaseParam.backRatio");

    // Translate EaseParam
    translateEaseParam_.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "TranslateEaseParam.isScaleEase");
    translateEaseParam_.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "TranslateEaseParam.maxTime");
    translateEaseParam_.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "TranslateEaseParam.easeTypeInt");
    translateEaseParam_.endValue.max          = globalParam->GetValue<Vector3>(groupName, "TranslateEaseParam.endValueV3.max");
    translateEaseParam_.endValue.min          = globalParam->GetValue<Vector3>(groupName, "TranslateEaseParam.endValueV3.min");
    translateEaseParam_.baseParam.backRatio   = globalParam->GetValue<float>(groupName, "TranslateEaseParam.backRatio");

    // Rotate EaseParam
    rotateEaseParam_.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "RotateEaseParam.isScaleEase");
    rotateEaseParam_.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "RotateEaseParam.maxTime");
    rotateEaseParam_.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "RotateEaseParam.easeTypeInt");
    rotateEaseParam_.endValue.max          = globalParam->GetValue<Vector3>(groupName, "RotateEaseParam.endValueV3.max");
    rotateEaseParam_.endValue.min          = globalParam->GetValue<Vector3>(groupName, "RotateEaseParam.endValueV3.min");
    rotateEaseParam_.baseParam.backRatio   = globalParam->GetValue<float>(groupName, "RotateEaseParam.backRatio");
}

void SRTParameters::AdjustParam() {
#ifdef _DEBUG
    ScaleParamEditor();
    RotateParamEditor();
    PositionParamEditor();
    VelocityParamEditor();
#endif
}

void SRTParameters::ScaleParamEditor() {
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");
        ImGui::Checkbox("IsScalerScale", &isScalerScale_);

        if (isScalerScale_) {
            ImGui::SeparatorText("Scaler Range");
            ImGui::DragFloat("Scale Max", &scaleDist_.max, 0.01f);
            ImGui::DragFloat("Scale Min", &scaleDist_.min, 0.01f);
        } else {
            ImGui::SeparatorText("V3 Range");
            ImGui::DragFloat3("ScaleV3 Max", &scaleDistV3_.max.x, 0.01f);
            ImGui::DragFloat3("ScaleV3 Min", &scaleDistV3_.min.x, 0.01f);
        }

        // Scale Easing
        ImGui::Checkbox("Use Scale Easing", &scaleEaseParam_.baseParam.isEase);

        if (scaleEaseParam_.baseParam.isEase) {
            ImGui::DragFloat("Max Time", &scaleEaseParam_.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("Easing Type", scaleEaseParam_.baseParam.easeTypeInt);

            if (isScalerScale_) {
                ImGui::DragFloat("End Scale Max", &scaleEaseParam_.endValueF.max, 0.01f);
                ImGui::DragFloat("End Scale Min", &scaleEaseParam_.endValueF.min, 0.01f);
            } else {
                ImGui::DragFloat3("End Scale Max", &scaleEaseParam_.endValueV3.max.x, 0.01f);
                ImGui::DragFloat3("End Scale Min", &scaleEaseParam_.endValueV3.min.x, 0.01f);
            }

            ImGui::DragFloat("Back Ratio", &scaleEaseParam_.baseParam.backRatio, 0.01f, 0.0f, 5.0f);
        }
    }
}

void SRTParameters::RotateParamEditor() {
    if (ImGui::CollapsingHeader("Rotate(Degree)")) {
        ImGui::DragFloat3("BaseRotate", &baseRotate_.x, 0.1f, -360, 360);
        ImGui::DragFloat3("Rotate Max", &rotateDist_.max.x, 0.1f, -360, 360);
        ImGui::DragFloat3("Rotate Min", &rotateDist_.min.x, 0.1f, -360, 360);
    }

    if (ImGui::CollapsingHeader("Rotate Speed(Degree)")) {
        ImGui::DragFloat3("Rotate Speed Max", &rotateSpeedDist_.max.x, 0.1f, 0, 720);
        ImGui::DragFloat3("Rotate Speed Min", &rotateSpeedDist_.min.x, 0.1f, 0, 720);
    }

    if (ImGui::CollapsingHeader("Rotate Flags")) {
        ImGui::Checkbox("IsRotateForDirection", &isRotateForDirection_);
    }
}

void SRTParameters::PositionParamEditor() {
    if (ImGui::CollapsingHeader("Position")) {
        ImGui::SeparatorText("Position Base:");
        ImGui::DragFloat3("Base", &emitPos_.x, 0.01f);

        ImGui::SeparatorText("Position Range:");
        ImGui::DragFloat3("Position Max", &positionDist_.max.x, 0.01f);
        ImGui::DragFloat3("Position Min", &positionDist_.min.x, 0.01f);
    }
}

void SRTParameters::VelocityParamEditor() {
    if (ImGui::CollapsingHeader("Velocity")) {
        ImGui::Checkbox("IsFloatVelocity", &isFloatVelocity_);
        if (isFloatVelocity_) {
            ImGui::SeparatorText("Velocity Range:");
            ImGui::DragFloat("Velocity Max", &speedDist_.max, 0.01f);
            ImGui::DragFloat("Velocity Min", &speedDist_.min, 0.01f);
        } else {
            ImGui::SeparatorText("V3 VelocityRange");
            ImGui::DragFloat3("VelocityV3 Max", &velocityDistV3_.max.x, 0.01f);
            ImGui::DragFloat3("VelocityV3 Min", &velocityDistV3_.min.x, 0.01f);
        }

        ImGui::SeparatorText("Direction Range:");
        ImGui::DragFloat3("Direction Max", &directionDist_.max.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat3("Direction Min", &directionDist_.min.x, 0.01f, -1.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Gravity")) {
        ImGui::DragFloat("Gravity", &gravity_, 0.1f);
    }
}