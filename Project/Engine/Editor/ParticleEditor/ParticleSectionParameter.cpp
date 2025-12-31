#include "ParticleSectionParameter.h"
#include "base/TextureManager.h"
#include "Function/GetFile.h"
#include "ParticleManager.h"
#include <imgui.h>

using namespace KetaEngine;

// ImGuiEasingTypeSelectorは削除 - EasingParameterData.hのものを使用

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
    globalParam->Regist(groupName, "UV Pos", &parameters_.uvParm.pos);
    globalParam->Regist(groupName, "UV Rotate", &parameters_.uvParm.rotate);
    globalParam->Regist(groupName, "UV NumOfFrame", &parameters_.uvParm.numOfFrame);
    globalParam->Regist(groupName, "UV ScroolSpeed", &parameters_.uvParm.frameScroolSpeed);
    globalParam->Regist(groupName, "UV IsRoop", &parameters_.uvParm.isRoop);
    globalParam->Regist(groupName, "UV isScroolEachPixel", &parameters_.uvParm.isScroolEachPixel);
    globalParam->Regist(groupName, "UV isScrool", &parameters_.uvParm.isScrool);
    globalParam->Regist(groupName, "UV isFlipX", &parameters_.uvParm.isFlipX);
    globalParam->Regist(groupName, "UV isFlipY", &parameters_.uvParm.isFlipY);

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

    // その他
    globalParam->Regist(groupName, "IntervalTime", &intervalTime_);
    globalParam->Regist(groupName, "Gravity", &parameters_.gravity);
    globalParam->Regist(groupName, "LifeTime", &parameters_.lifeTime);
    globalParam->Regist(groupName, "Particle Count", &particleCount_);
    globalParam->Regist(groupName, "Max Particle Num", &maxParticleNum_);

    // Flag
    globalParam->Regist(groupName, "isScalerScale", &parameters_.isScalerScale);
    globalParam->Regist(groupName, "isRotateforDirection", &parameters_.isRotateforDirection);
    globalParam->Regist(groupName, "isBillBord", &groupParamaters_.isBillBord);
    globalParam->Regist(groupName, "AdaptRotateIsX", &groupParamaters_.adaptRotate_.isX);
    globalParam->Regist(groupName, "AdaptRotateIsY", &groupParamaters_.adaptRotate_.isY);
    globalParam->Regist(groupName, "AdaptRotateIsZ", &groupParamaters_.adaptRotate_.isZ);
    globalParam->Regist(groupName, "isShot", &isShot_);
    globalParam->Regist(groupName, "isAlphaNoMove", &groupParamaters_.isAlphaNoMove);

    // EaseParm
    globalParam->Regist(groupName, "scaleEaseParm.isScaleEase", &parameters_.scaleEaseParm.isScaleEase);
    globalParam->Regist(groupName, "scaleEaseParm.maxTime", &parameters_.scaleEaseParm.maxTime);
    globalParam->Regist(groupName, "scaleEaseParm.easeTypeInt", &parameters_.scaleEaseParm.easeTypeInt);
    globalParam->Regist(groupName, "scaleEaseParm.startValueF", &parameters_.scaleEaseParm.startValueF);
    globalParam->Regist(groupName, "scaleEaseParm.endValueF.max", &parameters_.scaleEaseParm.endValueF.max);
    globalParam->Regist(groupName, "scaleEaseParm.endValueF.min", &parameters_.scaleEaseParm.endValueF.min);
    globalParam->Regist(groupName, "scaleEaseParm.startValueV3", &parameters_.scaleEaseParm.startValueV3);
    globalParam->Regist(groupName, "scaleEaseParm.endValueV3.max", &parameters_.scaleEaseParm.endValueV3.max);
    globalParam->Regist(groupName, "scaleEaseParm.endValueV3.min", &parameters_.scaleEaseParm.endValueV3.min);
    globalParam->Regist(groupName, "scaleEaseParm.backRatio", &parameters_.scaleEaseParm.backRatio);
    globalParam->Regist(groupName, "scaleEaseParm.currentScaleOffset", &parameters_.scaleEaseParm.currentScaleOffset);

    // Mode Setting
    globalParam->Regist(groupName, "preBillBordType_", &billBordType_);
    globalParam->Regist(groupName, "blendMode", &blendMode_);

    // Texture
    globalParam->Regist(groupName, "selectedTexturePath_", &selectedTexturePath_);

    // Primitive
    globalParam->Regist(groupName, "useModel", &useModel_);
    globalParam->Regist(groupName, "primitiveTypeInt", &primitiveTypeInt_);
    globalParam->Regist(groupName, "modelFilePath", &modelFilePath_);

    // Timing Parameter
    globalParam->Regist(groupName, "StartTime", &timingParam_.startTime);

    // TimeMode Parameter
    timeModeSelector_.RegisterParam(groupName, globalParam);

    // Rail Parameter
    globalParam->Regist(groupName, "UseRail", &sectionParam_.useRail);
    globalParam->Regist(groupName, "RailFileName", &railFileParam_.first);

    // Scale Easing Parameter
    globalParam->Regist(groupName, "UseScaleEasing", &sectionParam_.useScaleEasing);
}

void ParticleSectionParameter::GetParams(GlobalParameter* globalParam, const std::string& groupName) {
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
    parameters_.uvParm.pos               = globalParam->GetValue<Vector2>(groupName, "UV Pos");
    parameters_.uvParm.rotate            = globalParam->GetValue<Vector3>(groupName, "UV Rotate");
    parameters_.uvParm.numOfFrame        = globalParam->GetValue<int32_t>(groupName, "UV NumOfFrame");
    parameters_.uvParm.frameScroolSpeed  = globalParam->GetValue<float>(groupName, "UV ScroolSpeed");
    parameters_.uvParm.isRoop            = globalParam->GetValue<bool>(groupName, "UV IsRoop");
    parameters_.uvParm.isScroolEachPixel = globalParam->GetValue<bool>(groupName, "UV isScroolEachPixel");
    parameters_.uvParm.isScrool          = globalParam->GetValue<bool>(groupName, "UV isScrool");
    parameters_.uvParm.isFlipX           = globalParam->GetValue<bool>(groupName, "UV isFlipX");
    parameters_.uvParm.isFlipY           = globalParam->GetValue<bool>(groupName, "UV isFlipY");

    // Velocity
    parameters_.speedDist.max      = globalParam->GetValue<float>(groupName, "Speed Max");
    parameters_.speedDist.min      = globalParam->GetValue<float>(groupName, "Speed Min");
    parameters_.velocityDistV3.max = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Max");
    parameters_.velocityDistV3.min = globalParam->GetValue<Vector3>(groupName, "SpeedV3 Min");
    parameters_.directionDist.max  = globalParam->GetValue<Vector3>(groupName, "Direction Max");
    parameters_.directionDist.min  = globalParam->GetValue<Vector3>(groupName, "Direction Min");
    parameters_.isFloatVelocity    = globalParam->GetValue<bool>(groupName, "isFloatVelocity");

    // Color
    parameters_.baseColor     = globalParam->GetValue<Vector4>(groupName, "BaseColor");
    parameters_.colorDist.max = globalParam->GetValue<Vector4>(groupName, "Color Max");
    parameters_.colorDist.min = globalParam->GetValue<Vector4>(groupName, "Color Min");

    // その他
    intervalTime_        = globalParam->GetValue<float>(groupName, "IntervalTime");
    parameters_.gravity  = globalParam->GetValue<float>(groupName, "Gravity");
    parameters_.lifeTime = globalParam->GetValue<float>(groupName, "LifeTime");
    particleCount_       = globalParam->GetValue<int32_t>(groupName, "Particle Count");
    maxParticleNum_      = globalParam->GetValue<int32_t>(groupName, "Max Particle Num");

    // Flag
    parameters_.isScalerScale         = globalParam->GetValue<bool>(groupName, "isScalerScale");
    parameters_.isRotateforDirection  = globalParam->GetValue<bool>(groupName, "isRotateforDirection");
    groupParamaters_.isBillBord       = globalParam->GetValue<bool>(groupName, "isBillBord");
    groupParamaters_.adaptRotate_.isX = globalParam->GetValue<bool>(groupName, "AdaptRotateIsX");
    groupParamaters_.adaptRotate_.isY = globalParam->GetValue<bool>(groupName, "AdaptRotateIsY");
    groupParamaters_.adaptRotate_.isZ = globalParam->GetValue<bool>(groupName, "AdaptRotateIsZ");
    isShot_                           = globalParam->GetValue<bool>(groupName, "isShot");
    groupParamaters_.isAlphaNoMove    = globalParam->GetValue<bool>(groupName, "isAlphaNoMove");

    // EaseParam
    parameters_.scaleEaseParm.isScaleEase        = globalParam->GetValue<bool>(groupName, "scaleEaseParm.isScaleEase");
    parameters_.scaleEaseParm.maxTime            = globalParam->GetValue<float>(groupName, "scaleEaseParm.maxTime");
    parameters_.scaleEaseParm.easeTypeInt        = globalParam->GetValue<int32_t>(groupName, "scaleEaseParm.easeTypeInt");
    parameters_.scaleEaseParm.startValueF        = globalParam->GetValue<float>(groupName, "scaleEaseParm.startValueF");
    parameters_.scaleEaseParm.endValueF.max      = globalParam->GetValue<float>(groupName, "scaleEaseParm.endValueF.max");
    parameters_.scaleEaseParm.endValueF.min      = globalParam->GetValue<float>(groupName, "scaleEaseParm.endValueF.min");
    parameters_.scaleEaseParm.startValueV3       = globalParam->GetValue<Vector3>(groupName, "scaleEaseParm.startValueV3");
    parameters_.scaleEaseParm.endValueV3.max     = globalParam->GetValue<Vector3>(groupName, "scaleEaseParm.endValueV3.max");
    parameters_.scaleEaseParm.endValueV3.min     = globalParam->GetValue<Vector3>(groupName, "scaleEaseParm.endValueV3.min");
    parameters_.scaleEaseParm.backRatio          = globalParam->GetValue<float>(groupName, "scaleEaseParm.backRatio");
    parameters_.scaleEaseParm.currentScaleOffset = globalParam->GetValue<Vector3>(groupName, "scaleEaseParm.currentScaleOffset");

    // Mode
    billBordType_ = globalParam->GetValue<int>(groupName, "preBillBordType_");
    blendMode_    = globalParam->GetValue<int>(groupName, "blendMode");

    // Texture
    selectedTexturePath_ = globalParam->GetValue<std::string>(groupName, "selectedTexturePath_");

    // Primitive
    useModel_         = globalParam->GetValue<bool>(groupName, "useModel");
    primitiveTypeInt_ = globalParam->GetValue<int32_t>(groupName, "primitiveTypeInt");
    modelFilePath_    = globalParam->GetValue<std::string>(groupName, "modelFilePath");

    // Apply loaded values
    groupParamaters_.blendMode    = static_cast<BlendMode>(blendMode_);
    groupParamaters_.billBordType = static_cast<BillboardType>(billBordType_);
    groupParamaters_.isShot       = isShot_;
}

void ParticleSectionParameter::AdjustParam() {
#ifdef _DEBUG
    ImGui::PushID((groupName_ + "_AllParams").c_str());

    // Timing Parameters
    if (ImGui::CollapsingHeader("Timing Settings")) {
        ImGui::DragFloat("Start Time", &timingParam_.startTime, 0.01f, 0.0f, 100.0f);
        timeModeSelector_.SelectTimeModeImGui("Time Mode");
    }

    // Rail Settings
    if (ImGui::CollapsingHeader("Rail Settings")) {
        ImGui::Checkbox("Use Rail", &sectionParam_.useRail);
        if (sectionParam_.useRail) {
            SelectRailFile("Rail File", railFolderPath_, railFileParam_);
        }
    }

    // Scale Easing
    if (ImGui::CollapsingHeader("Scale Easing")) {
        ImGui::Checkbox("Use Scale Easing", &sectionParam_.useScaleEasing);

        if (sectionParam_.useScaleEasing) {
            auto& easeParm = parameters_.scaleEaseParm;

            ImGui::DragFloat("Max Time", &easeParm.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("Easing Type", easeParm.easeTypeInt);

            if (parameters_.isScalerScale) {
                ImGui::DragFloat("Start Scale", &easeParm.startValueF, 0.01f);
                ImGui::DragFloat("End Scale Max", &easeParm.endValueF.max, 0.01f);
            } else {
                ImGui::DragFloat3("Start Scale", &easeParm.startValueV3.x, 0.01f);
                ImGui::DragFloat3("End Scale Max", &easeParm.endValueV3.max.x, 0.01f);
            }

            ImGui::DragFloat("Back Ratio", &easeParm.backRatio, 0.01f, 0.0f, 5.0f);
        }
    }

    // Color
    if (ImGui::CollapsingHeader("Color")) {
        ImGui::SeparatorText("Base Color:");
        ImGui::ColorEdit4("Base", &parameters_.baseColor.x);

        ImGui::SeparatorText("Color Range:");
        ImGui::ColorEdit4("Max", &parameters_.colorDist.max.x);
        ImGui::ColorEdit4("Min", &parameters_.colorDist.min.x);
    }

    // Position
    if (ImGui::CollapsingHeader("Position")) {
        ImGui::SeparatorText("Position Base:");
        ImGui::DragFloat3("Base", &parameters_.emitPos.x, 0.1f);

        ImGui::SeparatorText("Position Range:");
        ImGui::DragFloat3("Position Max", &parameters_.positionDist.max.x, 0.1f);
        ImGui::DragFloat3("Position Min", &parameters_.positionDist.min.x, 0.1f);
    }

    // Velocity
    if (ImGui::CollapsingHeader("Velocity")) {
        ImGui::Checkbox("IsFloatVelocity", &parameters_.isFloatVelocity);
        if (parameters_.isFloatVelocity) {
            ImGui::SeparatorText("Velocity Range:");
            ImGui::DragFloat("Velocity Max", &parameters_.speedDist.max, 0.1f);
            ImGui::DragFloat("Velocity Min", &parameters_.speedDist.min, 0.1f);
        } else {
            ImGui::SeparatorText("V3 VelocityRange");
            ImGui::DragFloat3("VelocityV3 Max", reinterpret_cast<float*>(&parameters_.velocityDistV3.max), 0.1f);
            ImGui::DragFloat3("VelocityV3 Min", reinterpret_cast<float*>(&parameters_.velocityDistV3.min), 0.1f);
        }

        ImGui::SeparatorText("Direction Range:");
        ImGui::DragFloat3("Direction Max", &parameters_.directionDist.max.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat3("Direction Min", &parameters_.directionDist.min.x, 0.01f, -1.0f, 1.0f);
    }

    // Scale
    ScaleParmEditor();

    // Rotate
    if (ImGui::CollapsingHeader("Rotate(Degree)")) {
        ImGui::DragFloat3("BaseRotate", &parameters_.baseRotate.x, 0.1f, -360, 360);
        ImGui::DragFloat3("Rotate Max", &parameters_.rotateDist.max.x, 0.1f, -360, 360);
        ImGui::DragFloat3("Rotate Min", &parameters_.rotateDist.min.x, 0.1f, -360, 360);
    }

    if (ImGui::CollapsingHeader("Rotate Speed(Degree)")) {
        ImGui::DragFloat3("Rotate Speed Max", &parameters_.rotateSpeedDist.max.x, 0.1f, 0, 720);
        ImGui::DragFloat3("Rotate Speed Min", &parameters_.rotateSpeedDist.min.x, 0.1f, 0, 720);
    }

    // UV
    if (ImGui::CollapsingHeader("UV Parameters")) {
        ImGui::SeparatorText("UV Position:");
        ImGui::DragFloat2("UV_Pos", &parameters_.uvParm.pos.x, 0.01f);

        ImGui::SeparatorText("UV Rotation:");
        ImGui::DragFloat3("UV_Rotate", &parameters_.uvParm.rotate.x, 0.1f);

        ImGui::SeparatorText("UV Animation:");
        ImGui::InputInt("Num of Frames", &parameters_.uvParm.numOfFrame);
        ImGui::DragFloat("Scrool Speed", &parameters_.uvParm.frameScroolSpeed, 0.01f);
        ImGui::Checkbox("Is Roop", &parameters_.uvParm.isRoop);
        ImGui::Checkbox("Is ScroolEachPixel", &parameters_.uvParm.isScroolEachPixel);
        ImGui::Checkbox("Is Scrool", &parameters_.uvParm.isScrool);
        ImGui::Checkbox("Is IsFlipX", &parameters_.uvParm.isFlipX);
        ImGui::Checkbox("Is IsFlipY", &parameters_.uvParm.isFlipY);
    }

    // その他のパラメータ
    if (ImGui::CollapsingHeader("etcParamater")) {
        ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("Gravity", &parameters_.gravity, 0.1f);
        ImGui::DragFloat("LifeTime", &parameters_.lifeTime, 0.01f);
        ImGui::SliderInt("Particle Count", &particleCount_, 1, 100);
    }

    // Billbord
    if (ImGui::CollapsingHeader("BillBoard")) {
        ImGui::Checkbox("IsBillBoard", &groupParamaters_.isBillBord);

        ImGui::SeparatorText("IsRotateAdapt");
        ImGui::Checkbox("IsX", &groupParamaters_.adaptRotate_.isX);
        ImGui::Checkbox("IsY", &groupParamaters_.adaptRotate_.isY);
        ImGui::Checkbox("IsZ", &groupParamaters_.adaptRotate_.isZ);

        ImGui::SeparatorText("BillBordType");
        const char* billBordItems[] = {"XYZ", "X", "Y", "Z"};
        if (ImGui::Combo("Billboard Type", &billBordType_, billBordItems, IM_ARRAYSIZE(billBordItems))) {
            groupParamaters_.billBordType = static_cast<BillboardType>(billBordType_);
        }
    }

    // blend mode
    if (ImGui::CollapsingHeader("BlendMode")) {
        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("Blend Mode", &blendMode_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            groupParamaters_.blendMode = static_cast<BlendMode>(blendMode_);
        }
    }

    // frag setting
    if (ImGui::CollapsingHeader("Frag")) {
        ImGui::Checkbox("IsRotateforDirection", &parameters_.isRotateforDirection);
        ImGui::Checkbox("IsShot", &isShot_);
        ImGui::Checkbox("isAlphaNoMove", &groupParamaters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    ImGui::PopID();
#endif
}

void ParticleSectionParameter::ScaleParmEditor() {
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");
        ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);

        if (parameters_.isScalerScale) {
            ImGui::SeparatorText("Scaler Range");
            ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.1f);
            ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.1f);
        } else {
            ImGui::SeparatorText("V3 Range");
            ImGui::DragFloat3("ScaleV3 Max", reinterpret_cast<float*>(&parameters_.scaleDistV3.max), 0.1f);
            ImGui::DragFloat3("ScaleV3 Min", reinterpret_cast<float*>(&parameters_.scaleDistV3.min), 0.1f);
        }
    }
}

void ParticleSectionParameter::ImGuiTextureSelection() {
    static int selectedIndex           = 0;
    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    DisplayFileSelection("SelectTexture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);
        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}

void ParticleSectionParameter::DisplayFileSelection(
    const std::string& header,
    const std::vector<std::string>& filenames,
    int& selectedIndex,
    const std::function<void(const std::string&)>& onApply) {

    if (!filenames.empty()) {
        std::vector<const char*> names;
        for (const auto& file : filenames) {
            names.push_back(file.c_str());
        }

        if (ImGui::CollapsingHeader(header.c_str())) {
            ImGui::ListBox(header.c_str(), &selectedIndex, names.data(), static_cast<int>(names.size()));

            if (ImGui::Button(("Apply:" + header).c_str())) {
                onApply(filenames[selectedIndex]);
            }
        }
    } else {
        ImGui::Text("No %s files found.", header.c_str());
    }
}

void ParticleSectionParameter::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
}

void ParticleSectionParameter::InitAdaptTexture() {
    if (selectedTexturePath_.empty()) {
        return;
    }
}

void ParticleSectionParameter::SelectRailFile(
    const char* label,
    const std::string& directory,
    std::pair<std::string, FileSelector>& param) {

    param.second.SelectFile(label, directory, param.first, "", false);
}