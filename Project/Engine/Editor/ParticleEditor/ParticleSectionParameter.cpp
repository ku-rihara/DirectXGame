#include "ParticleSectionParameter.h"
#include "base/TextureManager.h"
#include "Function/GetFile.h"
#include "ParticleManager.h"
#include <imgui.h>

using namespace KetaEngine;

void ParticleSectionParameter::AdaptIntToType() {
    groupParameters_.blendMode    = static_cast<BlendMode>(blendModeInt_);
    groupParameters_.billBordType = static_cast<BillboardType>(groupParameters_.billBordType);
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

    // その他
    globalParam->Regist(groupName, "IntervalTime", &intervalTime_);
    globalParam->Regist(groupName, "Gravity", &parameters_.gravity);
    globalParam->Regist(groupName, "LifeTime", &parameters_.lifeTime);
    globalParam->Regist(groupName, "Particle Count", &particleCount_);
    globalParam->Regist(groupName, "Max Particle Num", &maxParticleNum_);

    // Flag
    globalParam->Regist(groupName, "isScalerScale", &parameters_.isScalerScale);
    globalParam->Regist(groupName, "isRotateForDirection", &parameters_.isRotateForDirection);
    globalParam->Regist(groupName, "isBillBord", &groupParameters_.isBillBord);
    globalParam->Regist(groupName, "AdaptRotateIsX", &groupParameters_.adaptRotate_.isX);
    globalParam->Regist(groupName, "AdaptRotateIsY", &groupParameters_.adaptRotate_.isY);
    globalParam->Regist(groupName, "AdaptRotateIsZ", &groupParameters_.adaptRotate_.isZ);
    globalParam->Regist(groupName, "isShot", &isShot_);
    globalParam->Regist(groupName, "isAlphaNoMove", &groupParameters_.isAlphaNoMove);

    // EaseParam
    globalParam->Regist(groupName, "scaleEaseParam.isScaleEase", &parameters_.scaleEaseParam.isScaleEase);
    globalParam->Regist(groupName, "scaleEaseParam.maxTime", &parameters_.scaleEaseParam.maxTime);
    globalParam->Regist(groupName, "scaleEaseParam.easeTypeInt", &parameters_.scaleEaseParam.easeTypeInt);
    globalParam->Regist(groupName, "scaleEaseParam.startValueF", &parameters_.scaleEaseParam.startValueF);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.max", &parameters_.scaleEaseParam.endValueF.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueF.min", &parameters_.scaleEaseParam.endValueF.min);
    globalParam->Regist(groupName, "scaleEaseParam.startValueV3", &parameters_.scaleEaseParam.startValueV3);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.max", &parameters_.scaleEaseParam.endValueV3.max);
    globalParam->Regist(groupName, "scaleEaseParam.endValueV3.min", &parameters_.scaleEaseParam.endValueV3.min);
    globalParam->Regist(groupName, "scaleEaseParam.backRatio", &parameters_.scaleEaseParam.backRatio);

    // Mode Setting
    globalParam->Regist(groupName, "preBillBordType", &billBordTypeInt_);
    globalParam->Regist(groupName, "blendMode", &blendModeInt_);

    // Texture
    globalParam->Regist(groupName, "selectedTexturePath", &selectedTexturePath_);

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
    parameters_.isRotateForDirection  = globalParam->GetValue<bool>(groupName, "isRotateForDirection");
    groupParameters_.isBillBord       = globalParam->GetValue<bool>(groupName, "isBillBord");
    groupParameters_.adaptRotate_.isX = globalParam->GetValue<bool>(groupName, "AdaptRotateIsX");
    groupParameters_.adaptRotate_.isY = globalParam->GetValue<bool>(groupName, "AdaptRotateIsY");
    groupParameters_.adaptRotate_.isZ = globalParam->GetValue<bool>(groupName, "AdaptRotateIsZ");
    isShot_                           = globalParam->GetValue<bool>(groupName, "isShot");
    groupParameters_.isAlphaNoMove    = globalParam->GetValue<bool>(groupName, "isAlphaNoMove");

    // EaseParam
    parameters_.scaleEaseParam.isScaleEase    = globalParam->GetValue<bool>(groupName, "scaleEaseParam.isScaleEase");
    parameters_.scaleEaseParam.maxTime        = globalParam->GetValue<float>(groupName, "scaleEaseParam.maxTime");
    parameters_.scaleEaseParam.easeTypeInt    = globalParam->GetValue<int32_t>(groupName, "scaleEaseParam.easeTypeInt");
    parameters_.scaleEaseParam.startValueF    = globalParam->GetValue<float>(groupName, "scaleEaseParam.startValueF");
    parameters_.scaleEaseParam.endValueF.max  = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.max");
    parameters_.scaleEaseParam.endValueF.min  = globalParam->GetValue<float>(groupName, "scaleEaseParam.endValueF.min");
    parameters_.scaleEaseParam.startValueV3   = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.startValueV3");
    parameters_.scaleEaseParam.endValueV3.max = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.max");
    parameters_.scaleEaseParam.endValueV3.min = globalParam->GetValue<Vector3>(groupName, "scaleEaseParam.endValueV3.min");
    parameters_.scaleEaseParam.backRatio      = globalParam->GetValue<float>(groupName, "scaleEaseParam.backRatio");

    // Mode
    billBordTypeInt_ = globalParam->GetValue<int>(groupName, "preBillBordType");
    blendModeInt_    = globalParam->GetValue<int>(groupName, "blendMode");

    // Texture
    selectedTexturePath_ = globalParam->GetValue<std::string>(groupName, "selectedTexturePath");

    // Primitive
    useModel_         = globalParam->GetValue<bool>(groupName, "useModel");
    primitiveTypeInt_ = globalParam->GetValue<int32_t>(groupName, "primitiveTypeInt");
    modelFilePath_    = globalParam->GetValue<std::string>(groupName, "modelFilePath");

    // Timing Parameter
    timingParam_.startTime = globalParam->GetValue<float>(groupName, "StartTime");
    timingParam_.afterDuration = globalParam->GetValue<float>(groupName, "afterDuration");

    // TimeMode Parameter
    timeModeSelector_.GetParam(groupName, globalParam);

    // Rail Parameter
    useRailMoveEmitter_  = globalParam->GetValue<bool>(groupName, "UseRail");
    railFileParam_.first = globalParam->GetValue<std::string>(groupName, "RailFileName");

    // Apply loaded values
    groupParameters_.blendMode    = static_cast<BlendMode>(blendModeInt_);
    groupParameters_.billBordType = static_cast<BillboardType>(billBordTypeInt_);
    groupParameters_.isShot       = isShot_;

    AdaptIntToType();
}

void ParticleSectionParameter::AdjustParam() {
#ifdef _DEBUG
    ImGui::PushID((groupName_ + "_AllParams").c_str());

    // Timing Parameters
    if (ImGui::CollapsingHeader("Timing Settings")) {
        ImGui::DragFloat("Start Time", &timingParam_.startTime, 0.01f, 0.0f, 100.0f);
        timeModeSelector_.SelectTimeModeImGui("Time Mode");
        ImGui::DragFloat("afterDuration", &timingParam_.afterDuration,0.01f);
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
            // モデルモード 
            ImGui::Text("Model File Path:");
            ImGui::Text("Format: ModelFolder/ModelName");
            ImGui::Text("Example: Suzanne/Suzanne");

            // std::string用のバッファを用意
            static char buffer[256] = "";

            // 初回またはモデルパスが変わった場合、バッファに反映
            static std::string lastPath = "";
            if (lastPath != modelFilePath_) {
                strncpy_s(buffer, sizeof(buffer), modelFilePath_.c_str(), sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0';
                lastPath                   = modelFilePath_;
            }

            // InputTextで編集
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
            const char* primitiveItems[] = {"Plane", "Sphere","Cylinder", "Ring", "Box"};

          
            if (ImGui::Combo("Primitive Type", &primitiveTypeInt_, primitiveItems, IM_ARRAYSIZE(primitiveItems))) {
                // Comboが変更された時、コールバックを呼び出す
                if (onPrimitiveChanged_) {
                    onPrimitiveChanged_(static_cast<PrimitiveType>(primitiveTypeInt_));
                }
            }
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
        ImGui::DragFloat3("Base", &parameters_.emitPos.x, 0.01f);

        ImGui::SeparatorText("Position Range:");
        ImGui::DragFloat3("Position Max", &parameters_.positionDist.max.x, 0.01f);
        ImGui::DragFloat3("Position Min", &parameters_.positionDist.min.x, 0.01f);
    }

    // Velocity
    if (ImGui::CollapsingHeader("Velocity")) {
        ImGui::Checkbox("IsFloatVelocity", &parameters_.isFloatVelocity);
        if (parameters_.isFloatVelocity) {
            ImGui::SeparatorText("Velocity Range:");
            ImGui::DragFloat("Velocity Max", &parameters_.speedDist.max, 0.01f);
            ImGui::DragFloat("Velocity Min", &parameters_.speedDist.min, 0.01f);
        } else {
            ImGui::SeparatorText("V3 VelocityRange");
            ImGui::DragFloat3("VelocityV3 Max", &parameters_.velocityDistV3.max.x, 0.01f);
            ImGui::DragFloat3("VelocityV3 Min", &parameters_.velocityDistV3.min.x, 0.01f);
        }

        ImGui::SeparatorText("Direction Range:");
        ImGui::DragFloat3("Direction Max", &parameters_.directionDist.max.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat3("Direction Min", &parameters_.directionDist.min.x, 0.01f, -1.0f, 1.0f);
    }

    // Scale
    ScaleParamEditor();

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
        ImGui::DragFloat2("UV_Pos", &parameters_.uvParam.pos.x, 0.01f);

        ImGui::SeparatorText("UV Rotation:");
        ImGui::DragFloat3("UV_Rotate", &parameters_.uvParam.rotate.x, 0.1f);

        ImGui::SeparatorText("UV Animation:");
        ImGui::InputInt("Num of Frames", &parameters_.uvParam.numOfFrame);
        ImGui::DragFloat("Scroll Speed", &parameters_.uvParam.frameScrollSpeed, 0.01f);
        ImGui::Checkbox("Is Loop", &parameters_.uvParam.isLoop);
        ImGui::Checkbox("Is ScrollEachPixel", &parameters_.uvParam.isScrollEachPixel);
        ImGui::Checkbox("Is Scroll", &parameters_.uvParam.isScroll);
        ImGui::Checkbox("Is IsFlipX", &parameters_.uvParam.isFlipX);
        ImGui::Checkbox("Is IsFlipY", &parameters_.uvParam.isFlipY);
    }

    // その他のパラメータ
    if (ImGui::CollapsingHeader("etcParameter")) {
        ImGui::DragFloat("IntervalTime", &intervalTime_, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("Gravity", &parameters_.gravity, 0.1f);
        ImGui::DragFloat("LifeTime", &parameters_.lifeTime, 0.01f);
        ImGui::SliderInt("Particle Count", &particleCount_, 1, 100);
    }

    // BillBord
    if (ImGui::CollapsingHeader("BillBoard")) {
        ImGui::Checkbox("IsBillBoard", &groupParameters_.isBillBord);

        ImGui::SeparatorText("IsRotateAdapt");
        ImGui::Checkbox("IsX", &groupParameters_.adaptRotate_.isX);
        ImGui::Checkbox("IsY", &groupParameters_.adaptRotate_.isY);
        ImGui::Checkbox("IsZ", &groupParameters_.adaptRotate_.isZ);

        ImGui::SeparatorText("BillBordType");
        const char* billBordItems[] = {"XYZ", "Y"};
        if (ImGui::Combo("Billboard Type", &billBordTypeInt_, billBordItems, IM_ARRAYSIZE(billBordItems))) {
            groupParameters_.billBordType = static_cast<BillboardType>(billBordTypeInt_);
        }
    }

    // blend mode
    if (ImGui::CollapsingHeader("BlendMode")) {
        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("Blend Mode", &blendModeInt_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            groupParameters_.blendMode = static_cast<BlendMode>(blendModeInt_);
        }
    }

    // frag setting
    if (ImGui::CollapsingHeader("Frag")) {
        ImGui::Checkbox("IsRotateForDirection", &parameters_.isRotateForDirection);
        ImGui::Checkbox("IsShot", &isShot_);
        ImGui::Checkbox("isAlphaNoMove", &groupParameters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    ImGui::PopID();
#endif
}
void ParticleSectionParameter::ScaleParamEditor() {
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");
        ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);

        if (parameters_.isScalerScale) {
            ImGui::SeparatorText("Scaler Range");
            ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.01f);
            ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.01f);
        } else {
            ImGui::SeparatorText("V3 Range");
            ImGui::DragFloat3("ScaleV3 Max", &parameters_.scaleDistV3.max.x, 0.01f);
            ImGui::DragFloat3("ScaleV3 Min", &parameters_.scaleDistV3.min.x, 0.01f);
        }

        // Scale Easing
        ImGui::Checkbox("Use Scale Easing", &parameters_.scaleEaseParam.isScaleEase);

        if (parameters_.scaleEaseParam.isScaleEase) {
            auto& easeParam = parameters_.scaleEaseParam;

            ImGui::DragFloat("Max Time", &easeParam.maxTime, 0.01f, 0.0f, 10.0f);
            ImGuiEasingTypeSelector("Easing Type", easeParam.easeTypeInt);

            if (parameters_.isScalerScale) {
                ImGui::DragFloat("End Scale Max", &easeParam.endValueF.max, 0.01f);
                ImGui::DragFloat("End Scale Min", &easeParam.endValueF.min, 0.01f);
            } else {
                ImGui::DragFloat3("End Scale Max", &easeParam.endValueV3.max.x, 0.01f);
                ImGui::DragFloat3("End Scale Min", &easeParam.endValueV3.min.x, 0.01f);
            }

            ImGui::DragFloat("Back Ratio", &easeParam.backRatio, 0.01f, 0.0f, 5.0f);
        }
    }
}

void ParticleSectionParameter::ImGuiTextureSelection() {
    static int selectedIndex               = 0;
    static std::string lastSelectedTexture = "";

    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    if (ImGui::CollapsingHeader("SelectTexture")) {
        if (!filenames.empty()) {
            std::vector<const char*> names;
            for (const auto& file : filenames) {
                names.push_back(file.c_str());
            }

            // ListBoxで選択
            if (ImGui::ListBox("Textures", &selectedIndex, names.data(), static_cast<int>(names.size()))) {
                // 選択が変わったら即座に適用
                std::string newTextureName = filenames[selectedIndex];
                if (newTextureName != lastSelectedTexture) {
                    ApplyTexture(newTextureName);
                    lastSelectedTexture = newTextureName;

                    // コールバック関数を呼び出して実際のテクスチャを更新
                    if (onTextureChanged_) {
                        onTextureChanged_();
                    }
                }
            }

            // 現在のテクスチャパスを表示
            if (!selectedTexturePath_.empty()) {
                ImGui::Text("Current: %s", selectedTexturePath_.c_str());
            }
        } else {
            ImGui::Text("No texture files found.");
        }
    }
}

void ParticleSectionParameter::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
}

void ParticleSectionParameter::InitAdaptTexture() {
    if (selectedTexturePath_.empty()) {
        selectedTexturePath_ = textureFilePath_ + "/" + "uvChecker" + ".png";
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
    billBordTypeInt_              = static_cast<int32_t>(type);
    groupParameters_.billBordType = type;
}

void ParticleSectionParameter::SetParentTransform(const WorldTransform* transform) {
    parameters_.parentTransform = transform;
}

void ParticleSectionParameter::SetParentJoint(const Object3DAnimation* animation, const std::string& jointName) {
    parameters_.jointParent.animation = animation;
    parameters_.jointParent.name      = jointName;
}

void ParticleSectionParameter::SetFollowingPos(const Vector3* pos) {
    parameters_.followingPos_ = pos;
}

void ParticleSectionParameter::SetTargetPosition(const Vector3& targetPos) {
    parameters_.targetPos = targetPos;
}

void ParticleSectionParameter::SetTextureChangedCallback(std::function<void()> callback) {
    onTextureChanged_ = callback;
}