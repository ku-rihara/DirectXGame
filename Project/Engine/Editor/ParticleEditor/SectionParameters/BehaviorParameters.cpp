#include "BehaviorParameters.h"
#include <imgui.h>

using namespace KetaEngine;

void BehaviorParameters::RegisterParams(GlobalParameter* globalParam, const std::string& groupName) {
    // Time parameters
    globalParam->Regist(groupName, "IntervalTime", &intervalTime_);
    globalParam->Regist(groupName, "LifeTime", &lifeTime_);
    globalParam->Regist(groupName, "Particle Count", &particleCount_);
    globalParam->Regist(groupName, "Max Particle Num", &maxParticleNum_);

    // Timing Parameter
    globalParam->Regist(groupName, "StartTime", &timingParam_.startTime);
    globalParam->Regist(groupName, "afterDuration", &timingParam_.afterDuration);

    // TimeMode Parameter
    timeModeSelector_.RegisterParam(groupName, globalParam);

    // Rail Parameter
    globalParam->Regist(groupName, "UseRail", &useRailMoveEmitter_);
    globalParam->Regist(groupName, "RailFileName", &railFileParam_.first);

    // Primitive/Model
    globalParam->Regist(groupName, "useModel", &useModel_);
    globalParam->Regist(groupName, "primitiveTypeInt", &primitiveTypeInt_);
    globalParam->Regist(groupName, "modelFilePath", &modelFilePath_);

    // Flags
    globalParam->Regist(groupName, "isShot", &isShot_);
    globalParam->Regist(groupName, "isBillBord", &isBillBord_);
    globalParam->Regist(groupName, "isAlphaNoMove", &isAlphaNoMove_);
    globalParam->Regist(groupName, "AdaptRotateIsX", &adaptRotate_.isX);
    globalParam->Regist(groupName, "AdaptRotateIsY", &adaptRotate_.isY);
    globalParam->Regist(groupName, "AdaptRotateIsZ", &adaptRotate_.isZ);

    // Mode Setting
    globalParam->Regist(groupName, "preBillBordType", &billBordTypeInt_);
    globalParam->Regist(groupName, "blendMode", &blendModeInt_);
}

void BehaviorParameters::AdaptParameters(GlobalParameter* globalParam, const std::string& groupName) {
    // Time parameters
    intervalTime_   = globalParam->GetValue<float>(groupName, "IntervalTime");
    lifeTime_       = globalParam->GetValue<float>(groupName, "LifeTime");
    particleCount_  = globalParam->GetValue<int32_t>(groupName, "Particle Count");
    maxParticleNum_ = globalParam->GetValue<int32_t>(groupName, "Max Particle Num");

    // Timing Parameter
    timingParam_.startTime     = globalParam->GetValue<float>(groupName, "StartTime");
    timingParam_.afterDuration = globalParam->GetValue<float>(groupName, "afterDuration");

    // TimeMode Parameter
    timeModeSelector_.GetParam(groupName, globalParam);

    // Rail Parameter
    useRailMoveEmitter_  = globalParam->GetValue<bool>(groupName, "UseRail");
    railFileParam_.first = globalParam->GetValue<std::string>(groupName, "RailFileName");

    // Primitive/Model
    useModel_         = globalParam->GetValue<bool>(groupName, "useModel");
    primitiveTypeInt_ = globalParam->GetValue<int32_t>(groupName, "primitiveTypeInt");
    modelFilePath_    = globalParam->GetValue<std::string>(groupName, "modelFilePath");

    // Flags
    isShot_          = globalParam->GetValue<bool>(groupName, "isShot");
    isBillBord_      = globalParam->GetValue<bool>(groupName, "isBillBord");
    isAlphaNoMove_   = globalParam->GetValue<bool>(groupName, "isAlphaNoMove");
    adaptRotate_.isX = globalParam->GetValue<bool>(groupName, "AdaptRotateIsX");
    adaptRotate_.isY = globalParam->GetValue<bool>(groupName, "AdaptRotateIsY");
    adaptRotate_.isZ = globalParam->GetValue<bool>(groupName, "AdaptRotateIsZ");

    // Mode
    billBordTypeInt_ = globalParam->GetValue<int>(groupName, "preBillBordType");
    blendModeInt_    = globalParam->GetValue<int>(groupName, "blendMode");
}

void BehaviorParameters::AdjustParam() {
#ifdef _DEBUG
    // Timing Parameters
    if (ImGui::CollapsingHeader("Timing Settings")) {
        ImGui::DragFloat("Start Time", &timingParam_.startTime, 0.01f, 0.0f, 100.0f);
        timeModeSelector_.SelectTimeModeImGui("Time Mode");
        ImGui::DragFloat("afterDuration", &timingParam_.afterDuration, 0.01f);
    }

    // Rail Settings
    if (ImGui::CollapsingHeader("Rail Settings")) {
        ImGui::Checkbox("Use Rail", &useRailMoveEmitter_);
        if (useRailMoveEmitter_) {
            SelectRailFile("Rail File", railFolderPath_, &railFileParam_);
        }
    }

    // Primitive/Model Settings
    if (ImGui::CollapsingHeader("Primitive/Model Type")) {
        ImGui::Checkbox("Use Model", &useModel_);

        if (useModel_) {
            ImGui::Text("Model File Path:");
            ImGui::Text("Format: ModelFolder/ModelName");
            ImGui::Text("Example: Suzanne/Suzanne");

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
            const char* primitiveItems[] = {"Plane", "Sphere", "Cylinder", "Ring", "Box"};

            if (ImGui::Combo("Primitive Type", &primitiveTypeInt_, primitiveItems, IM_ARRAYSIZE(primitiveItems))) {
                if (onPrimitiveChanged_) {
                    onPrimitiveChanged_(static_cast<PrimitiveType>(primitiveTypeInt_));
                }
            }
        }
    }

    // etc Parameter
    if (ImGui::CollapsingHeader("etcParameter")) {
        ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("LifeTime", &lifeTime_, 0.01f);
        ImGui::SliderInt("Particle Count", &particleCount_, 1, 100);
    }

    // BillBord
    if (ImGui::CollapsingHeader("BillBoard")) {
        ImGui::Checkbox("IsBillBoard", &isBillBord_);

        ImGui::SeparatorText("IsRotateAdapt");
        ImGui::Checkbox("IsX", &adaptRotate_.isX);
        ImGui::Checkbox("IsY", &adaptRotate_.isY);
        ImGui::Checkbox("IsZ", &adaptRotate_.isZ);

        ImGui::SeparatorText("BillBordType");
        const char* billBordItems[] = {"XYZ", "Y"};
        if (ImGui::Combo("Billboard Type", &billBordTypeInt_, billBordItems, IM_ARRAYSIZE(billBordItems))) {
            // Type changed
        }
    }

    // blend mode
    if (ImGui::CollapsingHeader("BlendMode")) {
        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("Blend Mode", &blendModeInt_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            // Mode changed
        }
    }

    // flag setting
    if (ImGui::CollapsingHeader("Frag")) {
        ImGui::Checkbox("IsShot", &isShot_);
        ImGui::Checkbox("isAlphaNoMove", &isAlphaNoMove_);
    }
#endif
}

void BehaviorParameters::SelectRailFile(
    const char* label,
    const std::string& directory,
    std::pair<std::string, FileSelector>* param) {

    param->second.SelectFile(label, directory, param->first, "", false);
}