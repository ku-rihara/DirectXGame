#include "ParticleSectionParameter.h"
#include "../../ParticleManager.h"
#include "Base/TextureManager.h"
#include "Editor/EffectEditorSuite/EffectInlineEditRequest.h"
#include "Function/GetFile.h"
#include <imgui.h>

using namespace KetaEngine;

void ParticleSectionParameter::AdaptIntToType() {
    groupParameters_.blendMode     = static_cast<BlendMode>(blendModeInt_);
    groupParameters_.billboardType = static_cast<BillboardType>(groupParameters_.billboardType);
    emitPositionMode_              = static_cast<EmitterPositionMode>(emitPositionModeInt_);
}

void ParticleSectionParameter::RegisterParams(GlobalParameter* globalParam, const std::string& groupName) {
    groupName_ = groupName;

    // Position
    globalParam->Regist(groupName, "Position Base", &parameters_.emitPos);
    globalParam->Regist(groupName, "Position Max", &parameters_.positionDist.max);
    globalParam->Regist(groupName, "Position Min", &parameters_.positionDist.min);

    // Scale
    globalParam->Regist(groupName, "Scale Max", &parameters_.scaleDist.max);
    globalParam->Regist(groupName, "Scale Min", &parameters_.scaleDist.min);
    globalParam->Regist(groupName, "ScaleV3 Max", &parameters_.scaleDistV3.max);
    globalParam->Regist(groupName, "ScaleV3 Min", &parameters_.scaleDistV3.min);

    // Rotate
    globalParam->Regist(groupName, "Rotate Base", &parameters_.baseRotate);
    globalParam->Regist(groupName, "Rotate Max", &parameters_.rotateDist.max);
    globalParam->Regist(groupName, "Rotate Min", &parameters_.rotateDist.min);
    globalParam->Regist(groupName, "RotateSpeed Max", &parameters_.rotateSpeedDist.max);
    globalParam->Regist(groupName, "RotateSpeed Min", &parameters_.rotateSpeedDist.min);

    // UV
    globalParam->Regist(groupName, "UV Pos", &parameters_.uvParam.pos);
    globalParam->Regist(groupName, "UV Rotate", &parameters_.uvParam.rotate);
    globalParam->Regist(groupName, "UV NumOfFrame", &parameters_.uvParam.numOfFrame);
    globalParam->Regist(groupName, "UV ScrollSpeed", &parameters_.uvParam.frameScrollSpeed);
    globalParam->Regist(groupName, "UV IsLoop", &parameters_.uvParam.isLoop);
    globalParam->Regist(groupName, "UV isScrollEachPixel", &parameters_.uvParam.isScrollEachPixel);
    globalParam->Regist(groupName, "UV isScroll", &parameters_.uvParam.isScroll);
    globalParam->Regist(groupName, "UV isFlipX", &parameters_.uvParam.isFlipX);
    globalParam->Regist(groupName, "UV isFlipY", &parameters_.uvParam.isFlipY);

    // Velocity
    globalParam->Regist(groupName, "Speed Max", &parameters_.speedDist.max);
    globalParam->Regist(groupName, "Speed Min", &parameters_.speedDist.min);
    globalParam->Regist(groupName, "SpeedV3 Max", &parameters_.velocityDistV3.max);
    globalParam->Regist(groupName, "SpeedV3 Min", &parameters_.velocityDistV3.min);
    globalParam->Regist(groupName, "Direction Max", &parameters_.directionDist.max);
    globalParam->Regist(groupName, "Direction Min", &parameters_.directionDist.min);
    globalParam->Regist(groupName, "isFloatVelocity", &parameters_.isFloatVelocity);

    // Color
    globalParam->Regist(groupName, "BaseColor", &parameters_.baseColor);
    globalParam->Regist(groupName, "Color Max", &parameters_.colorDist.max);
    globalParam->Regist(groupName, "Color Min", &parameters_.colorDist.min);
    globalParam->Regist(groupName, "ColorEaseParam.isEase", &parameters_.colorEaseParam.baseParam.isEase);
    globalParam->Regist(groupName, "ColorEaseParam.maxTime", &parameters_.colorEaseParam.baseParam.maxTime);
    globalParam->Regist(groupName, "ColorEaseParam.easeTypeInt", &parameters_.colorEaseParam.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "ColorEaseParam.endValue.max", &parameters_.colorEaseParam.endValue.max);
    globalParam->Regist(groupName, "ColorEaseParam.endValue.min", &parameters_.colorEaseParam.endValue.min);
    globalParam->Regist(groupName, "alphaMode", &alphaModeInt_);
    globalParam->Regist(groupName, "AlphaEaseParam.isEase", &parameters_.alphaEaseParam.baseParam.isEase);
    globalParam->Regist(groupName, "AlphaEaseParam.maxTime", &parameters_.alphaEaseParam.baseParam.maxTime);
    globalParam->Regist(groupName, "AlphaEaseParam.easeTypeInt", &parameters_.alphaEaseParam.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "AlphaEaseParam.endValue.max", &parameters_.alphaEaseParam.endValue.max);
    globalParam->Regist(groupName, "AlphaEaseParam.endValue.min", &parameters_.alphaEaseParam.endValue.min);

    // その他
    globalParam->Regist(groupName, "IntervalTime", &intervalTime_);
    globalParam->Regist(groupName, "Gravity", &parameters_.gravity);
    globalParam->Regist(groupName, "LifeTime", &parameters_.lifeTime);
    globalParam->Regist(groupName, "Particle Count", &particleCount_);
    globalParam->Regist(groupName, "Max Particle Num", &maxParticleNum_);
    globalParam->Regist(groupName, "IsScreenPos", &groupParameters_.isScreenPos);

    // Flag
    globalParam->Regist(groupName, "isScalerScale", &parameters_.isScalerScale);
    globalParam->Regist(groupName, "isRotateForDirection", &parameters_.isRotateForDirection);
    globalParam->Regist(groupName, "isBillboard", &groupParameters_.isBillboard);
    globalParam->Regist(groupName, "AdaptRotateIsX", &groupParameters_.adaptRotate_.isX);
    globalParam->Regist(groupName, "AdaptRotateIsY", &groupParameters_.adaptRotate_.isY);
    globalParam->Regist(groupName, "AdaptRotateIsZ", &groupParameters_.adaptRotate_.isZ);
    globalParam->Regist(groupName, "isShot", &isShot_);
    globalParam->Regist(groupName, "isAlphaNoMove", &groupParameters_.isAlphaNoMove);

    // EaseParam
    globalParam->Regist(groupName, "scaleEaseParam.isScaleEase", &parameters_.scaleEaseParam.baseParam.isEase);
    globalParam->Regist(groupName, "scaleEaseParam.maxTime", &parameters_.scaleEaseParam.baseParam.maxTime);
    globalParam->Regist(groupName, "scaleEaseParam.easeTypeInt", &parameters_.scaleEaseParam.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.max", &parameters_.scaleEaseParam.endValueF.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.min", &parameters_.scaleEaseParam.endValueF.min);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.max", &parameters_.scaleEaseParam.endValueV3.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.min", &parameters_.scaleEaseParam.endValueV3.min);
    globalParam->Regist(groupName, "scaleEaseParam.returnMaxTime", &parameters_.scaleEaseParam.baseParam.returnMaxTime);
    globalParam->Regist(groupName, "scaleEaseParam.returnEaseTypeInt", &parameters_.scaleEaseParam.baseParam.returnEaseTypeInt);
    globalParam->Regist(groupName, "scaleEaseParam.isReturnToOrigin", &parameters_.scaleEaseParam.baseParam.isReturnToOrigin);

    // ease Translate
    globalParam->Regist(groupName, "TranslateEaseParam.isScaleEase", &parameters_.translateEaseParam.baseParam.isEase);
    globalParam->Regist(groupName, "TranslateEaseParam.maxTime", &parameters_.translateEaseParam.baseParam.maxTime);
    globalParam->Regist(groupName, "TranslateEaseParam.easeTypeInt", &parameters_.translateEaseParam.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "TranslateEaseParam.endValueV3.max", &parameters_.translateEaseParam.endValue.max);
    globalParam->Regist(groupName, "TranslateEaseParam.endValueV3.min", &parameters_.translateEaseParam.endValue.min);
    globalParam->Regist(groupName, "TranslateEaseParam.returnMaxTime", &parameters_.translateEaseParam.baseParam.returnMaxTime);
    globalParam->Regist(groupName, "TranslateEaseParam.returnEaseTypeInt", &parameters_.translateEaseParam.baseParam.returnEaseTypeInt);
    globalParam->Regist(groupName, "TranslateEaseParam.isReturnToOrigin", &parameters_.translateEaseParam.baseParam.isReturnToOrigin);

    // ease Rotate
    globalParam->Regist(groupName, "RotateEaseParam.isScaleEase", &parameters_.rotateEaseParam.baseParam.isEase);
    globalParam->Regist(groupName, "RotateEaseParam.maxTime", &parameters_.rotateEaseParam.baseParam.maxTime);
    globalParam->Regist(groupName, "RotateEaseParam.easeTypeInt", &parameters_.rotateEaseParam.baseParam.easeTypeInt);
    globalParam->Regist(groupName, "RotateEaseParam.endValueV3.max", &parameters_.rotateEaseParam.endValue.max);
    globalParam->Regist(groupName, "RotateEaseParam.endValueV3.min", &parameters_.rotateEaseParam.endValue.min);

    // Mode Setting
    globalParam->Regist(groupName, "preBillboardType", &billboardTypeInt_);
    globalParam->Regist(groupName, "blendMode", &blendModeInt_);

    // Texture
    globalParam->Regist(groupName, "selectedTexturePath", &selectedTexturePath_);

    // Distortion
    globalParam->Regist(groupName, "useDistortion", &groupParameters_.useDistortion);
    globalParam->Regist(groupName, "distortionStrength", &groupParameters_.distortionStrength);

    // Emissive
    globalParam->Regist(groupName, "emissiveIntensity", &groupParameters_.emissiveIntensity);
    globalParam->Regist(groupName, "distortionTexturePath", &distortionTexturePath_);

    // Dissolve
    globalParam->Regist(groupName, "dissolveName", &dissolveName_);

    // Primitive
    globalParam->Regist(groupName, "useModel", &useModel_);
    globalParam->Regist(groupName, "primitiveTypeInt", &primitiveTypeInt_);
    globalParam->Regist(groupName, "modelFilePath", &modelFilePath_);

    // Timing Parameter
    globalParam->Regist(groupName, "StartTime", &timingParam_.startTime);
    globalParam->Regist(groupName, "afterDuration", &timingParam_.afterDuration);

    // TimeMode Parameter
    timeModeSelector_.RegisterParam(groupName, globalParam);

    // Rail Parameter
    globalParam->Regist(groupName, "UseRail", &useRailMoveEmitter_);
    globalParam->Regist(groupName, "RailFileName", &railFileParam_.first);

    // Parent Setting Flags
    globalParam->Regist(groupName, "emitPositionMode", &emitPositionModeInt_);

    // Shape
    globalParam->Regist(groupName, "emitShape", &emitShapeInt_);
    globalParam->Regist(groupName, "sphereRadius.min", &parameters_.sphereRadius.min);
    globalParam->Regist(groupName, "sphereRadius.max", &parameters_.sphereRadius.max);

    AdaptIntToType();
}

void ParticleSectionParameter::AdaptParameters(GlobalParameter* globalParam, const std::string& groupName) {
    // Position
    parameters_.emitPos          = globalParam->GetValue<Vector3>(groupName, "Position Base");
    parameters_.positionDist.max = globalParam->GetValue<Vector3>(groupName, "Position Max");
    parameters_.positionDist.min = globalParam->GetValue<Vector3>(groupName, "Position Min");

    // Scale
    parameters_.scaleDist.max   = globalParam->GetValue<float>(groupName, "Scale Max");
    parameters_.scaleDist.min   = globalParam->GetValue<float>(groupName, "Scale Min");
    parameters_.scaleDistV3.max = globalParam->GetValue<Vector3>(groupName, "ScaleV3 Max");
    parameters_.scaleDistV3.min = globalParam->GetValue<Vector3>(groupName, "ScaleV3 Min");

    // Rotate
    parameters_.baseRotate          = globalParam->GetValue<Vector3>(groupName, "Rotate Base");
    parameters_.rotateDist.max      = globalParam->GetValue<Vector3>(groupName, "Rotate Max");
    parameters_.rotateDist.min      = globalParam->GetValue<Vector3>(groupName, "Rotate Min");
    parameters_.rotateSpeedDist.max = globalParam->GetValue<Vector3>(groupName, "RotateSpeed Max");
    parameters_.rotateSpeedDist.min = globalParam->GetValue<Vector3>(groupName, "RotateSpeed Min");

    // UV
    parameters_.uvParam.pos               = globalParam->GetValue<Vector2>(groupName, "UV Pos");
    parameters_.uvParam.rotate            = globalParam->GetValue<Vector3>(groupName, "UV Rotate");
    parameters_.uvParam.numOfFrame        = globalParam->GetValue<int32_t>(groupName, "UV NumOfFrame");
    parameters_.uvParam.frameScrollSpeed  = globalParam->GetValue<float>(groupName, "UV ScrollSpeed");
    parameters_.uvParam.isLoop            = globalParam->GetValue<bool>(groupName, "UV IsLoop");
    parameters_.uvParam.isScrollEachPixel = globalParam->GetValue<bool>(groupName, "UV isScrollEachPixel");
    parameters_.uvParam.isScroll          = globalParam->GetValue<bool>(groupName, "UV isScroll");
    parameters_.uvParam.isFlipX           = globalParam->GetValue<bool>(groupName, "UV isFlipX");
    parameters_.uvParam.isFlipY           = globalParam->GetValue<bool>(groupName, "UV isFlipY");

    // Velocity
    parameters_.speedDist.max      = globalParam->GetValue<float>(groupName, "Speed Max");
    parameters_.speedDist.min      = globalParam->GetValue<float>(groupName, "Speed Min");
    parameters_.velocityDistV3.max = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Max");
    parameters_.velocityDistV3.min = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Min");
    parameters_.directionDist.max  = globalParam->GetValue<Vector3>(groupName, "Direction Max");
    parameters_.directionDist.min  = globalParam->GetValue<Vector3>(groupName, "Direction Min");
    parameters_.isFloatVelocity    = globalParam->GetValue<bool>(groupName, "isFloatVelocity");

    // Color
    parameters_.baseColor                            = globalParam->GetValue<Vector4>(groupName, "BaseColor");
    parameters_.colorDist.max                        = globalParam->GetValue<Vector4>(groupName, "Color Max");
    parameters_.colorDist.min                        = globalParam->GetValue<Vector4>(groupName, "Color Min");
    parameters_.colorEaseParam.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "ColorEaseParam.isEase");
    parameters_.colorEaseParam.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "ColorEaseParam.maxTime");
    parameters_.colorEaseParam.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "ColorEaseParam.easeTypeInt");
    parameters_.colorEaseParam.endValue.max          = globalParam->GetValue<Vector3>(groupName, "ColorEaseParam.endValue.max");
    parameters_.colorEaseParam.endValue.min          = globalParam->GetValue<Vector3>(groupName, "ColorEaseParam.endValue.min");
    alphaModeInt_                                    = globalParam->GetValue<int32_t>(groupName, "alphaMode");
    parameters_.alphaMode                            = static_cast<ParticleCommon::AlphaMode>(alphaModeInt_);
    parameters_.alphaEaseParam.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "AlphaEaseParam.isEase");
    parameters_.alphaEaseParam.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "AlphaEaseParam.maxTime");
    parameters_.alphaEaseParam.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "AlphaEaseParam.easeTypeInt");
    parameters_.alphaEaseParam.endValue.max          = globalParam->GetValue<float>(groupName, "AlphaEaseParam.endValue.max");
    parameters_.alphaEaseParam.endValue.min          = globalParam->GetValue<float>(groupName, "AlphaEaseParam.endValue.min");

    // その他
    intervalTime_                = globalParam->GetValue<float>(groupName, "IntervalTime");
    parameters_.gravity          = globalParam->GetValue<float>(groupName, "Gravity");
    parameters_.lifeTime         = globalParam->GetValue<float>(groupName, "LifeTime");
    particleCount_               = globalParam->GetValue<int32_t>(groupName, "Particle Count");
    maxParticleNum_              = globalParam->GetValue<int32_t>(groupName, "Max Particle Num");
    groupParameters_.isScreenPos = globalParam->GetValue<bool>(groupName, "IsScreenPos");

    // Flag
    parameters_.isScalerScale         = globalParam->GetValue<bool>(groupName, "isScalerScale");
    parameters_.isRotateForDirection  = globalParam->GetValue<bool>(groupName, "isRotateForDirection");
    groupParameters_.isBillboard      = globalParam->GetValue<bool>(groupName, "isBillboard");
    groupParameters_.adaptRotate_.isX = globalParam->GetValue<bool>(groupName, "AdaptRotateIsX");
    groupParameters_.adaptRotate_.isY = globalParam->GetValue<bool>(groupName, "AdaptRotateIsY");
    groupParameters_.adaptRotate_.isZ = globalParam->GetValue<bool>(groupName, "AdaptRotateIsZ");
    isShot_                           = globalParam->GetValue<bool>(groupName, "isShot");
    groupParameters_.isAlphaNoMove    = globalParam->GetValue<bool>(groupName, "isAlphaNoMove");

    // EaseParam
    parameters_.scaleEaseParam.baseParam.isEase            = globalParam->GetValue<bool>(groupName, "scaleEaseParam.isScaleEase");
    parameters_.scaleEaseParam.baseParam.maxTime           = globalParam->GetValue<float>(groupName, "scaleEaseParam.maxTime");
    parameters_.scaleEaseParam.baseParam.easeTypeInt       = globalParam->GetValue<int32_t>(groupName, "scaleEaseParam.easeTypeInt");
    parameters_.scaleEaseParam.endValueF.max               = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.max");
    parameters_.scaleEaseParam.endValueF.min               = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.min");
    parameters_.scaleEaseParam.endValueV3.max              = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.max");
    parameters_.scaleEaseParam.endValueV3.min              = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.min");
    parameters_.scaleEaseParam.baseParam.returnMaxTime     = globalParam->GetValue<float>(groupName, "scaleEaseParam.returnMaxTime");
    parameters_.scaleEaseParam.baseParam.returnEaseTypeInt = globalParam->GetValue<int32_t>(groupName, "scaleEaseParam.returnEaseTypeInt");
    parameters_.scaleEaseParam.baseParam.isReturnToOrigin  = globalParam->GetValue<bool>(groupName, "scaleEaseParam.isReturnToOrigin");

    parameters_.translateEaseParam.baseParam.isEase            = globalParam->GetValue<bool>(groupName, "TranslateEaseParam.isScaleEase");
    parameters_.translateEaseParam.baseParam.maxTime           = globalParam->GetValue<float>(groupName, "TranslateEaseParam.maxTime");
    parameters_.translateEaseParam.baseParam.easeTypeInt       = globalParam->GetValue<int32_t>(groupName, "TranslateEaseParam.easeTypeInt");
    parameters_.translateEaseParam.endValue.max                = globalParam->GetValue<Vector3>(groupName, "TranslateEaseParam.endValueV3.max");
    parameters_.translateEaseParam.endValue.min                = globalParam->GetValue<Vector3>(groupName, "TranslateEaseParam.endValueV3.min");
    parameters_.translateEaseParam.baseParam.returnMaxTime     = globalParam->GetValue<float>(groupName, "TranslateEaseParam.returnMaxTime");
    parameters_.translateEaseParam.baseParam.returnEaseTypeInt = globalParam->GetValue<int32_t>(groupName, "TranslateEaseParam.returnEaseTypeInt");
    parameters_.translateEaseParam.baseParam.isReturnToOrigin  = globalParam->GetValue<bool>(groupName, "TranslateEaseParam.isReturnToOrigin");

    // ease Rotate
    parameters_.rotateEaseParam.baseParam.isEase      = globalParam->GetValue<bool>(groupName, "RotateEaseParam.isScaleEase");
    parameters_.rotateEaseParam.baseParam.maxTime     = globalParam->GetValue<float>(groupName, "RotateEaseParam.maxTime");
    parameters_.rotateEaseParam.baseParam.easeTypeInt = globalParam->GetValue<int32_t>(groupName, "RotateEaseParam.easeTypeInt");
    parameters_.rotateEaseParam.endValue.max          = globalParam->GetValue<Vector3>(groupName, "RotateEaseParam.endValueV3.max");
    parameters_.rotateEaseParam.endValue.min          = globalParam->GetValue<Vector3>(groupName, "RotateEaseParam.endValueV3.min");

    // Mode
    billboardTypeInt_ = globalParam->GetValue<int>(groupName, "preBillboardType");
    blendModeInt_     = globalParam->GetValue<int>(groupName, "blendMode");

    // Texture
    selectedTexturePath_ = globalParam->GetValue<std::string>(groupName, "selectedTexturePath");

    // Distortion
    groupParameters_.useDistortion      = globalParam->GetValue<bool>(groupName, "useDistortion");
    groupParameters_.distortionStrength = globalParam->GetValue<float>(groupName, "distortionStrength");

    // Emissive
    groupParameters_.emissiveIntensity = globalParam->GetValue<float>(groupName, "emissiveIntensity");
    distortionTexturePath_             = globalParam->GetValue<std::string>(groupName, "distortionTexturePath");

    // Dissolve
    dissolveName_ = globalParam->GetValue<std::string>(groupName, "dissolveName");

    // Primitive
    useModel_         = globalParam->GetValue<bool>(groupName, "useModel");
    primitiveTypeInt_ = globalParam->GetValue<int32_t>(groupName, "primitiveTypeInt");
    modelFilePath_    = globalParam->GetValue<std::string>(groupName, "modelFilePath");

    // Timing Parameter
    timingParam_.startTime     = globalParam->GetValue<float>(groupName, "StartTime");
    timingParam_.afterDuration = globalParam->GetValue<float>(groupName, "afterDuration");

    // TimeMode Parameter
    timeModeSelector_.GetParam(groupName, globalParam);

    // Rail Parameter
    useRailMoveEmitter_  = globalParam->GetValue<bool>(groupName, "UseRail");
    railFileParam_.first = globalParam->GetValue<std::string>(groupName, "RailFileName");

    // Parent Setting Flags
    emitPositionModeInt_ = globalParam->GetValue<int32_t>(groupName, "emitPositionMode");

    // Shape
    emitShapeInt_                = globalParam->GetValue<int32_t>(groupName, "emitShape");
    parameters_.emitShape        = static_cast<ParticleCommon::EmitShape>(emitShapeInt_);
    parameters_.sphereRadius.min = globalParam->GetValue<float>(groupName, "sphereRadius.min");
    parameters_.sphereRadius.max = globalParam->GetValue<float>(groupName, "sphereRadius.max");

    // Apply loaded values
    groupParameters_.blendMode     = static_cast<BlendMode>(blendModeInt_);
    groupParameters_.billboardType = static_cast<BillboardType>(billboardTypeInt_);
    groupParameters_.isShot        = isShot_;

    AdaptIntToType();
}

void ParticleSectionParameter::AdjustParam() {
#ifdef _DEBUG
    ImGui::PushID((groupName_ + "_AllParams").c_str());

    // Emit Position Mode
    ImGui::SeparatorText("位置モード");
    const char* PositionModeItems[] = {"None", "ParentTransform", "TargetPosition", "FollowingPosition", "ParentJoint"};
    if (ImGui::Combo("エミッター位置モード", &emitPositionModeInt_, PositionModeItems, IM_ARRAYSIZE(PositionModeItems))) {
        emitPositionMode_ = static_cast<EmitterPositionMode>(emitPositionModeInt_);
    }

    // Timing Parameters
    if (ImGui::CollapsingHeader("タイミング設定")) {
        ImGui::DragFloat("開始時間", &timingParam_.startTime, 0.01f, 0.0f, 100.0f);
        timeModeSelector_.SelectTimeModeImGui("タイムモード");
        ImGui::DragFloat("持続時間", &timingParam_.afterDuration, 0.01f);
    }

    // Rail Settings
    if (ImGui::CollapsingHeader("レール設定")) {
        ImGui::Checkbox("レール使用", &useRailMoveEmitter_);
        if (useRailMoveEmitter_) {
            SelectRailFile("レールファイル", railFolderPath_, &railFileParam_);
        }
    }

    // Primitive/Model Settings
    if (ImGui::CollapsingHeader("プリミティブ/モデルタイプ")) {
        ImGui::Checkbox("モデル使用", &useModel_);

        if (useModel_) {
            // モデルモード
            ImGui::Text("モデルファイルパス:");
            ImGui::Text("形式: モデルフォルダ/モデル名");

            static char buffer[256]     = "";
            static std::string lastPath = "";
            if (lastPath != modelFilePath_) {
                strncpy_s(buffer, sizeof(buffer), modelFilePath_.c_str(), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
                lastPath                   = modelFilePath_;
            }

            if (ImGui::InputText("##ModelPath", buffer, sizeof(buffer))) {
                modelFilePath_ = std::string(buffer);
                lastPath       = modelFilePath_;
            }

            if (!modelFilePath_.empty()) {
                std::string fullPath = modelBasePath_ + modelFilePath_ + "/" + modelFilePath_ + ".obj";
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Full: %s", fullPath.c_str());
            }
        } else {
            // プリミティブモード
            const char* primitiveItems[] = {"Plane", "Sphere", "Cylinder", "Ring", "Box"};

            if (ImGui::Combo("プリミティブタイプ", &primitiveTypeInt_, primitiveItems, IM_ARRAYSIZE(primitiveItems))) {
                if (onPrimitiveChanged_) {
                    onPrimitiveChanged_(static_cast<PrimitiveType>(primitiveTypeInt_));
                }
            }
        }
    }

    // Color
    if (ImGui::CollapsingHeader("カラー")) {
        ImGui::SeparatorText("ベースカラー:");
        ImGui::ColorEdit4("ベース", &parameters_.baseColor.x);

        ImGui::SeparatorText("カラー範囲:");
        ImGui::ColorEdit4("最大", &parameters_.colorDist.max.x);
        ImGui::ColorEdit4("最小", &parameters_.colorDist.min.x);

        // アルファモード
        ImGui::Separator();
        ImGui::SeparatorText("アルファ:");
        const char* alphaModeItems[] = {"None（消えない）", "LifeTime（時間経過）", "Easing（イージング）"};
        if (ImGui::Combo("アルファモード", &alphaModeInt_, alphaModeItems, IM_ARRAYSIZE(alphaModeItems))) {
            parameters_.alphaMode = static_cast<ParticleCommon::AlphaMode>(alphaModeInt_);
        }

        if (parameters_.alphaMode == ParticleCommon::AlphaMode::Easing) {
            auto& easeParam = parameters_.alphaEaseParam;
            ImGui::DragFloat("最大時間##Alpha", &easeParam.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("イージングタイプ##Alpha", easeParam.baseParam.easeTypeInt);
            ImGui::DragFloat("終了アルファ 最大", &easeParam.endValue.max, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("終了アルファ 最小", &easeParam.endValue.min, 0.01f, 0.0f, 1.0f);
        }

        ImGui::Separator();
        ImGui::Checkbox("カラーイージング使用", &parameters_.colorEaseParam.baseParam.isEase);

        if (parameters_.colorEaseParam.baseParam.isEase) {
            auto& easeParam = parameters_.colorEaseParam;

            ImGui::DragFloat("最大時間##Color", &easeParam.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("イージングタイプ##Color", easeParam.baseParam.easeTypeInt);

            ImGui::SeparatorText("終了カラー範囲（RGB）:");
            ImGui::ColorEdit3("終了カラー 最大", &easeParam.endValue.max.x);
            ImGui::ColorEdit3("終了カラー 最小", &easeParam.endValue.min.x);
        }
    }

    // Shape
    if (ImGui::CollapsingHeader("形状 (Shape)")) {
        const char* shapeItems[] = {"None（ベクトル）", "Sphere（球面放射）"};
        if (ImGui::Combo("エミット形状", &emitShapeInt_, shapeItems, IM_ARRAYSIZE(shapeItems))) {
            parameters_.emitShape = static_cast<ParticleCommon::EmitShape>(emitShapeInt_);
        }

        if (parameters_.emitShape == ParticleCommon::EmitShape::Sphere) {
            ImGui::SeparatorText("Sphere 設定");
            ImGui::DragFloat("半径 最小", &parameters_.sphereRadius.min, 0.01f, 0.0f, 100.0f);
            ImGui::DragFloat("半径 最大", &parameters_.sphereRadius.max, 0.01f, 0.0f, 100.0f);
        }
    }

    // Position
    if (ImGui::CollapsingHeader("位置")) {
        ImGui::SeparatorText("位置ベース:");
        ImGui::DragFloat3("ベース", &parameters_.emitPos.x, 0.01f);

        if (parameters_.emitShape == ParticleCommon::EmitShape::None) {
            ImGui::SeparatorText("位置範囲:");
            ImGui::DragFloat3("位置 最大", &parameters_.positionDist.max.x, 0.01f);
            ImGui::DragFloat3("位置 最小", &parameters_.positionDist.min.x, 0.01f);
        }
    }

    // Velocity
    if (ImGui::CollapsingHeader("速度")) {
        ImGui::Checkbox("スカラー速度", &parameters_.isFloatVelocity);
        if (parameters_.isFloatVelocity) {
            ImGui::SeparatorText("速度範囲:");
            ImGui::DragFloat("速度 最大", &parameters_.speedDist.max, 0.01f);
            ImGui::DragFloat("速度 最小", &parameters_.speedDist.min, 0.01f);
        } else {
            ImGui::SeparatorText("ベクトル速度範囲");
            ImGui::DragFloat3("速度V3 最大", &parameters_.velocityDistV3.max.x, 0.01f);
            ImGui::DragFloat3("速度V3 最小", &parameters_.velocityDistV3.min.x, 0.01f);
        }

        ImGui::SeparatorText("方向範囲:");
        ImGui::DragFloat3("方向 最大", &parameters_.directionDist.max.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat3("方向 最小", &parameters_.directionDist.min.x, 0.01f, -1.0f, 1.0f);

        TranslateParamEditor();
    }

    // Scale
    ScaleParamEditor();

    // Rotate
    if (ImGui::CollapsingHeader("回転（度）")) {
        ImGui::DragFloat3("基準回転", &parameters_.baseRotate.x, 0.1f, -360, 360);
        ImGui::DragFloat3("回転 最大", &parameters_.rotateDist.max.x, 0.1f, -360, 360);
        ImGui::DragFloat3("回転 最小", &parameters_.rotateDist.min.x, 0.1f, -360, 360);
    }

    if (ImGui::CollapsingHeader("回転速度（度）")) {
        ImGui::DragFloat3("回転速度 最大", &parameters_.rotateSpeedDist.max.x, 0.1f, 0, 720);
        ImGui::DragFloat3("回転速度 最小", &parameters_.rotateSpeedDist.min.x, 0.1f, 0, 720);
        RotateParamEditor();
    }

    // UV
    if (ImGui::CollapsingHeader("UVパラメータ")) {
        ImGui::SeparatorText("UV位置:");
        ImGui::DragFloat2("UV位置", &parameters_.uvParam.pos.x, 0.01f);

        ImGui::SeparatorText("UV回転:");
        ImGui::DragFloat3("UV回転", &parameters_.uvParam.rotate.x, 0.1f);

        ImGui::SeparatorText("UVアニメーション:");
        ImGui::InputInt("フレーム数", &parameters_.uvParam.numOfFrame);
        ImGui::DragFloat("スクロール速度", &parameters_.uvParam.frameScrollSpeed, 0.01f);
        ImGui::Checkbox("ループ", &parameters_.uvParam.isLoop);
        ImGui::Checkbox("ピクセル毎スクロール", &parameters_.uvParam.isScrollEachPixel);
        ImGui::Checkbox("スクロール", &parameters_.uvParam.isScroll);
        ImGui::Checkbox("X反転", &parameters_.uvParam.isFlipX);
        ImGui::Checkbox("Y反転", &parameters_.uvParam.isFlipY);
    }

    // その他のパラメータ
    if (ImGui::CollapsingHeader("その他のパラメータ")) {
        ImGui::DragFloat("発生間隔", &intervalTime_, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("重力", &parameters_.gravity, 0.1f);
        ImGui::DragFloat("寿命", &parameters_.lifeTime, 0.01f);
        ImGui::SliderInt("パーティクル数", &particleCount_, 1, 100);
        ImGui::Checkbox("スクリーン座標", &groupParameters_.isScreenPos);
    }

    // BillBord
    if (ImGui::CollapsingHeader("ビルボード")) {
        ImGui::Checkbox("ビルボード有効", &groupParameters_.isBillboard);

        ImGui::SeparatorText("回転適用軸");
        ImGui::Checkbox("X軸", &groupParameters_.adaptRotate_.isX);
        ImGui::Checkbox("Y軸", &groupParameters_.adaptRotate_.isY);
        ImGui::Checkbox("Z軸", &groupParameters_.adaptRotate_.isZ);

        ImGui::SeparatorText("ビルボードタイプ");
        const char* billBordItems[] = {"XYZ", "Y"};
        if (ImGui::Combo("ビルボードタイプ", &billboardTypeInt_, billBordItems, IM_ARRAYSIZE(billBordItems))) {
            groupParameters_.billboardType = static_cast<BillboardType>(billboardTypeInt_);
        }
    }

    // blend mode
    if (ImGui::CollapsingHeader("ブレンドモード設定")) {
        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("ブレンドモード", &blendModeInt_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            groupParameters_.blendMode = static_cast<BlendMode>(blendModeInt_);
        }
    }

    // frag setting
    if (ImGui::CollapsingHeader("フラグ")) {
        ImGui::Checkbox("方向に合わせて回転", &parameters_.isRotateForDirection);
        ImGui::Checkbox("ショット", &isShot_);
        ImGui::Checkbox("アルファのみ動かない", &groupParameters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    // 自己発光設定
    if (ImGui::CollapsingHeader("自己発光")) {
        ImGui::DragFloat("発光強度", &groupParameters_.emissiveIntensity, 0.1f, 0.0f, 20.0f);
    }

    // 歪みエフェクト設定
    if (ImGui::CollapsingHeader("時空歪み")) {
        ImGui::Checkbox("歪み有効", &groupParameters_.useDistortion);

        if (groupParameters_.useDistortion) {
            ImGui::DragFloat("歪み強度", &groupParameters_.distortionStrength, 0.001f, 0.0f, 1.0f);

            ImGui::Separator();
            ImGui::Text("歪みテクスチャ");
            {
                std::string prevDistortion = distortionTexturePath_;
                distortionTextureFileSelector_.SelectFilePath("##DistortionTexture", noiseFolderPath_, distortionTexturePath_, ".dds", true);
                if (distortionTexturePath_ != prevDistortion && onTextureChanged_) {
                    onTextureChanged_();
                }
            }
            if (!distortionTexturePath_.empty()) {
                ImGui::TextDisabled("Path: %s", distortionTexturePath_.c_str());
            }
        }
    }

    // Dissolve設定
    if (ImGui::CollapsingHeader("ディゾルブ")) {
        dissolveFileSelector_.SelectFile("ディゾルブ名", dissolveFolderPath_, dissolveName_, "", true);
        if (dissolveName_ == "None") {
            dissolveName_ = "";
        }

        if (!dissolveName_.empty()) {
            ImGui::Text("選択中: %s", dissolveName_.c_str());
            ImGui::SameLine();
            if (ImGui::Button("編集##DissolveEdit")) {
                EffectInlineEditRequest::Request("Dissolve", dissolveName_, "Common");
            }
        } else {
            ImGui::TextDisabled("Dissolveなし");
        }
    }

    ImGui::PopID();
#endif
}

void ParticleSectionParameter::ScaleParamEditor() {
    if (ImGui::CollapsingHeader("スケール")) {
        ImGui::SeparatorText("スケールモード");
        ImGui::Checkbox("スカラーモード", &parameters_.isScalerScale);

        if (parameters_.isScalerScale) {
            ImGui::SeparatorText("スカラー範囲");
            ImGui::DragFloat("スケール 最大", &parameters_.scaleDist.max, 0.01f);
            ImGui::DragFloat("スケール 最小", &parameters_.scaleDist.min, 0.01f);
        } else {
            ImGui::SeparatorText("ベクトル範囲");
            ImGui::DragFloat3("スケールV3 最大", &parameters_.scaleDistV3.max.x, 0.01f);
            ImGui::DragFloat3("スケールV3 最小", &parameters_.scaleDistV3.min.x, 0.01f);
        }

        // Scale Easing
        ImGui::Checkbox("スケールイージング使用", &parameters_.scaleEaseParam.baseParam.isEase);

        if (parameters_.scaleEaseParam.baseParam.isEase) {
            auto& easeParam = parameters_.scaleEaseParam;

            ImGui::DragFloat("最大時間", &easeParam.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("イージングタイプ", easeParam.baseParam.easeTypeInt);

            if (parameters_.isScalerScale) {
                ImGui::DragFloat("終了スケール 最大", &easeParam.endValueF.max, 0.01f);
                ImGui::DragFloat("終了スケール 最小", &easeParam.endValueF.min, 0.01f);
            } else {
                ImGui::DragFloat3("終了スケール 最大", &easeParam.endValueV3.max.x, 0.01f);
                ImGui::DragFloat3("終了スケール 最小", &easeParam.endValueV3.min.x, 0.01f);
            }

            ImGui::Checkbox("起点に戻る##Scale", &easeParam.baseParam.isReturnToOrigin);
            if (easeParam.baseParam.isReturnToOrigin) {
                ImGui::DragFloat("戻り時間##Scale", &easeParam.baseParam.returnMaxTime, 0.01f, 0.0f, 10.0f);
                ImGuiEasingTypeSelector("戻りイージングタイプ##Scale", easeParam.baseParam.returnEaseTypeInt);
            }
        }
    }
}

void ParticleSectionParameter::TranslateParamEditor() {

    ImGui::Separator();
    ImGui::Checkbox("移動イージング使用", &parameters_.translateEaseParam.baseParam.isEase);

    if (parameters_.translateEaseParam.baseParam.isEase) {
        auto& easeParam = parameters_.translateEaseParam;

        ImGui::DragFloat("最大時間##Translate", &easeParam.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("イージングタイプ##Translate", easeParam.baseParam.easeTypeInt);

        ImGui::SeparatorText("終了位置範囲");
        ImGui::DragFloat3("終了移動 最大", &easeParam.endValue.max.x, 0.01f);
        ImGui::DragFloat3("終了移動 最小", &easeParam.endValue.min.x, 0.01f);

        ImGui::Checkbox("起点に戻る##Translate", &easeParam.baseParam.isReturnToOrigin);
        if (easeParam.baseParam.isReturnToOrigin) {
            ImGui::DragFloat("戻り時間##Translate", &easeParam.baseParam.returnMaxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("戻りイージングタイプ##Translate", easeParam.baseParam.returnEaseTypeInt);
        }
    }
}

void ParticleSectionParameter::RotateParamEditor() {
    ImGui::Separator();

    ImGui::Checkbox("回転イージング使用", &parameters_.rotateEaseParam.baseParam.isEase);

    if (parameters_.rotateEaseParam.baseParam.isEase) {
        auto& easeParam = parameters_.rotateEaseParam;

        ImGui::DragFloat("最大時間##Rotate", &easeParam.baseParam.maxTime, 0.01f, 0.0f, 10.0f);
        ImGuiEasingTypeSelector("イージングタイプ##Rotate", easeParam.baseParam.easeTypeInt);

        ImGui::SeparatorText("終了回転範囲（度）");
        ImGui::DragFloat3("終了回転 最大", &easeParam.endValue.max.x, 0.1f, -360.0f, 360.0f);
        ImGui::DragFloat3("終了回転 最小", &easeParam.endValue.min.x, 0.1f, -360.0f, 360.0f);
    }
}

void ParticleSectionParameter::ImGuiTextureSelection() {
    if (ImGui::CollapsingHeader("テクスチャ選択")) {
        std::string prev = selectedTexturePath_;
        textureFileSelector_.SelectFilePath("##TextureSelect", textureFilePath_, selectedTexturePath_, ".dds", false);
        if (selectedTexturePath_ != prev && onTextureChanged_) {
            onTextureChanged_();
        }
        if (!selectedTexturePath_.empty()) {
            ImGui::TextDisabled("Path: %s", selectedTexturePath_.c_str());
        }
    }
}

void ParticleSectionParameter::InitAdaptTexture() {
    if (selectedTexturePath_.empty()) {
        selectedTexturePath_ = textureFilePath_ + "/" + "uvChecker" + ".dds";
    }
}

void ParticleSectionParameter::SelectRailFile(
    const char* label,
    const std::string& directory,
    std::pair<std::string, FileSelector>* param) {

    param->second.SelectFile(label, directory, param->first, "", false);
}

void ParticleSectionParameter::SetIsShot(bool shot) {
    isShot_                 = shot;
    groupParameters_.isShot = shot;
}

void ParticleSectionParameter::SetBlendMode(BlendMode mode) {
    blendModeInt_              = static_cast<int32_t>(mode);
    groupParameters_.blendMode = mode;
}

void ParticleSectionParameter::SetBillboardType(BillboardType type) {
    billboardTypeInt_              = static_cast<int32_t>(type);
    groupParameters_.billboardType = type;
}

void ParticleSectionParameter::SetParentTransform(const WorldTransform* transform) {
    if (emitPositionMode_ != EmitterPositionMode::ParentTransform) {
        return;
    }
    parameters_.parentTransform = transform;
}

void ParticleSectionParameter::SetParentJoint(const Object3DAnimation* animation, const std::string& jointName) {
    if (emitPositionMode_ != EmitterPositionMode::ParentJoint) {
        return;
    }
    parameters_.jointParent.animation = animation;
    parameters_.jointParent.name      = jointName;
}

void ParticleSectionParameter::SetFollowingPos(const Vector3* pos) {
    if (emitPositionMode_ != EmitterPositionMode::FollowingPosition) {
        return;
    }
    parameters_.followingPos_ = pos;
}

void ParticleSectionParameter::SetTargetPosition(const Vector3& targetPos) {
    if (emitPositionMode_ != EmitterPositionMode::TargetPosition) {
        return;
    }
    parameters_.targetPos = targetPos;
}

void ParticleSectionParameter::SetTextureChangedCallback(std::function<void()> callback) {

    onTextureChanged_ = callback;
}