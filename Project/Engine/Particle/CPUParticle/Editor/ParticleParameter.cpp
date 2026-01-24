#include "ParticleParameter.h"

using namespace KetaEngine;
// Manager
#include "../ParticleManager.h"
// Base
#include "Base/TextureManager.h"
// Frame
#include "Frame/Frame.h"
// Function
#include "Function/GetFile.h"
// std
#include <format>
// imGui
#include <imgui.h>

void ParticleParameter::ParameterInit() {
    // パラメータ初期化
    parameters_.targetPos            = {0.0f, 0.0f, 0.0f};
    parameters_.lifeTime             = 0.0f;
    parameters_.gravity              = 0.0f;
    parameters_.baseColor            = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.colorDist.min        = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.colorDist.max        = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.isScalerScale        = true;
    parameters_.isFloatVelocity      = true;
    parameters_.isRotateForDirection = false;
    particleCount_                   = 0;
    intervalTime_                    = 1.0f;
    groupParamaters_.isBillBord      = true;
    parameters_.uvParam.numOfFrame   = 1;
    selectedTexturePath_             = "resources/Texture/uvChecker.png";
}

void ParticleParameter::Init() {
    ParameterInit();

    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(particleName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(particleName_);

    // Editorで設定したテクスチャを適用
    InitAdaptTexture();

    // セッティングしたもの適用
    groupParamaters_.billBordType = static_cast<BillboardType>(billBordType_);
    groupParamaters_.isShot       = isShot_;
    groupParamaters_.blendMode    = static_cast<BlendMode>(blendMode_);
    /*  parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);*/
}

void ParticleParameter::RegisterParams() {
    // Position
    globalParameter_->Regist(particleName_, "Position Base", &parameters_.emitPos);
    globalParameter_->Regist(particleName_, "Position Max", &parameters_.positionDist.max);
    globalParameter_->Regist(particleName_, "Position Min", &parameters_.positionDist.min);

    // Scale
    globalParameter_->Regist(particleName_, "Scale Max", &parameters_.scaleDist.max);
    globalParameter_->Regist(particleName_, "Scale Min", &parameters_.scaleDist.min);
    globalParameter_->Regist(particleName_, "ScaleV3 Max", &parameters_.scaleDistV3.max);
    globalParameter_->Regist(particleName_, "ScaleV3 Min", &parameters_.scaleDistV3.min);

    // Rotate
    globalParameter_->Regist(particleName_, "Rotate Base", &parameters_.baseRotate);
    globalParameter_->Regist(particleName_, "Rotate Max", &parameters_.rotateDist.max);
    globalParameter_->Regist(particleName_, "Rotate Min", &parameters_.rotateDist.min);
    globalParameter_->Regist(particleName_, "RotateSpeed Max", &parameters_.rotateSpeedDist.max);
    globalParameter_->Regist(particleName_, "RotateSpeed Min", &parameters_.rotateSpeedDist.min);

    // UV
    globalParameter_->Regist(particleName_, "UV Pos", &parameters_.uvParam.pos);
    globalParameter_->Regist(particleName_, "UV Rotate", &parameters_.uvParam.rotate);
    globalParameter_->Regist(particleName_, "UV NumOfFrame", &parameters_.uvParam.numOfFrame);
    globalParameter_->Regist(particleName_, "UV ScroolSpeed", &parameters_.uvParam.frameScrollSpeed);
    globalParameter_->Regist(particleName_, "UV IsRoop", &parameters_.uvParam.isLoop);
    globalParameter_->Regist(particleName_, "UV isScroolEachPixel", &parameters_.uvParam.isScrollEachPixel);
    globalParameter_->Regist(particleName_, "UV isScrool", &parameters_.uvParam.isScroll);
    globalParameter_->Regist(particleName_, "UV isFlipX", &parameters_.uvParam.isFlipX);
    globalParameter_->Regist(particleName_, "UV isFlipY", &parameters_.uvParam.isFlipY);

    // Velocity
    globalParameter_->Regist(particleName_, "Speed Max", &parameters_.speedDist.max);
    globalParameter_->Regist(particleName_, "Speed Min", &parameters_.speedDist.min);
    globalParameter_->Regist(particleName_, "SpeedV3 Max", &parameters_.velocityDistV3.max);
    globalParameter_->Regist(particleName_, "SpeedV3 Min", &parameters_.velocityDistV3.min);
    globalParameter_->Regist(particleName_, "Direction Max", &parameters_.directionDist.max);
    globalParameter_->Regist(particleName_, "Direction Min", &parameters_.directionDist.min);
    globalParameter_->Regist(particleName_, "isFloatVelocity", &parameters_.isFloatVelocity);

    // Color
    globalParameter_->Regist(particleName_, "BaseColor", &parameters_.baseColor);
    globalParameter_->Regist(particleName_, "Color Max", &parameters_.colorDist.max);
    globalParameter_->Regist(particleName_, "Color Min", &parameters_.colorDist.min);

    // その他
    globalParameter_->Regist(particleName_, "IntervalTime", &intervalTime_);
    globalParameter_->Regist(particleName_, "Gravity", &parameters_.gravity);
    globalParameter_->Regist(particleName_, "LifeTime", &parameters_.lifeTime);
    globalParameter_->Regist(particleName_, "Particle Count", &particleCount_);
    globalParameter_->Regist(particleName_, "Max Particle Num", &maxParticleNum_);

    // frag
    globalParameter_->Regist(particleName_, "isScalerScale", &parameters_.isScalerScale);
    globalParameter_->Regist(particleName_, "isRotateforDirection", &parameters_.isRotateForDirection);
    globalParameter_->Regist(particleName_, "isBillBord", &groupParamaters_.isBillBord);
    globalParameter_->Regist(particleName_, "AdaptRotateIsX", &groupParamaters_.adaptRotate_.isX);
    globalParameter_->Regist(particleName_, "AdaptRotateIsY", &groupParamaters_.adaptRotate_.isY);
    globalParameter_->Regist(particleName_, "AdaptRotateIsZ", &groupParamaters_.adaptRotate_.isZ);
    globalParameter_->Regist(particleName_, "isShot", &isShot_);
    globalParameter_->Regist(particleName_, "isAlphaNoMove", &groupParamaters_.isAlphaNoMove);

    globalParameter_->Regist(particleName_, "isRailRoop", &isRailRoop_);
    globalParameter_->Regist(particleName_, "isMoveForRail", &isMoveForRail_);
    globalParameter_->Regist(particleName_, "moveSpeed", &moveSpeed_);

    // easeParm
    globalParameter_->Regist(particleName_, "scaleEaseParm.isScaleEase", &parameters_.scaleEaseParam.baseParam.isEase);
    globalParameter_->Regist(particleName_, "scaleEaseParm.maxTime", &parameters_.scaleEaseParam.baseParam.maxTime);
    globalParameter_->Regist(particleName_, "scaleEaseParm.easeTypeInt", &parameters_.scaleEaseParam.baseParam.easeTypeInt);
    globalParameter_->Regist(particleName_, "scaleEaseParm.endValueF.max", &parameters_.scaleEaseParam.endValueF.max);
    globalParameter_->Regist(particleName_, "scaleEaseParm.endValueF.min", &parameters_.scaleEaseParam.endValueF.min);
    globalParameter_->Regist(particleName_, "scaleEaseParm.endValueV3.max", &parameters_.scaleEaseParam.endValueV3.max);
    globalParameter_->Regist(particleName_, "scaleEaseParm.endValueV3.min", &parameters_.scaleEaseParam.endValueV3.min);
    globalParameter_->Regist(particleName_, "scaleEaseParm.backRatio", &parameters_.scaleEaseParam.baseParam.backRatio);

    // mode Setting
    globalParameter_->Regist(particleName_, "preBillBordType_", &billBordType_);
    globalParameter_->Regist(particleName_, "blendMode", &blendMode_);

    // テクスチャ
    globalParameter_->Regist(particleName_, "selectedTexturePath_", &selectedTexturePath_);

    // Primitive
    globalParameter_->Regist(particleName_, "useModel", &useModel_);
    globalParameter_->Regist(particleName_, "primitiveTypeInt", &primitiveTypeInt_);
    globalParameter_->Regist(particleName_, "modelFilePath", &modelFilePath_);
}

void ParticleParameter::GetParams() {
    // Position
    parameters_.emitPos          = globalParameter_->GetValue<Vector3>(particleName_, "Position Base");
    parameters_.positionDist.max = globalParameter_->GetValue<Vector3>(particleName_, "Position Max");
    parameters_.positionDist.min = globalParameter_->GetValue<Vector3>(particleName_, "Position Min");

    // Scale
    parameters_.scaleDist.max   = globalParameter_->GetValue<float>(particleName_, "Scale Max");
    parameters_.scaleDist.min   = globalParameter_->GetValue<float>(particleName_, "Scale Min");
    parameters_.scaleDistV3.max = globalParameter_->GetValue<Vector3>(particleName_, "ScaleV3 Max");
    parameters_.scaleDistV3.min = globalParameter_->GetValue<Vector3>(particleName_, "ScaleV3 Min");

    // Rotate
    parameters_.baseRotate          = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Base");
    parameters_.rotateDist.max      = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Max");
    parameters_.rotateDist.min      = globalParameter_->GetValue<Vector3>(particleName_, "Rotate Min");
    parameters_.rotateSpeedDist.max = globalParameter_->GetValue<Vector3>(particleName_, "RotateSpeed Max");
    parameters_.rotateSpeedDist.min = globalParameter_->GetValue<Vector3>(particleName_, "RotateSpeed Min");

    // UV
    parameters_.uvParam.pos               = globalParameter_->GetValue<Vector2>(particleName_, "UV Pos");
    parameters_.uvParam.rotate            = globalParameter_->GetValue<Vector3>(particleName_, "UV Rotate");
    parameters_.uvParam.numOfFrame        = globalParameter_->GetValue<int32_t>(particleName_, "UV NumOfFrame");
    parameters_.uvParam.frameScrollSpeed  = globalParameter_->GetValue<float>(particleName_, "UV ScroolSpeed");
    parameters_.uvParam.isLoop            = globalParameter_->GetValue<bool>(particleName_, "UV IsRoop");
    parameters_.uvParam.isScrollEachPixel = globalParameter_->GetValue<bool>(particleName_, "UV isScroolEachPixel");
    parameters_.uvParam.isScroll          = globalParameter_->GetValue<bool>(particleName_, "UV isScrool");
    parameters_.uvParam.isFlipX           = globalParameter_->GetValue<bool>(particleName_, "UV isFlipX");
    parameters_.uvParam.isFlipY           = globalParameter_->GetValue<bool>(particleName_, "UV isFlipY");

    // Velocity
    parameters_.speedDist.max      = globalParameter_->GetValue<float>(particleName_, "Speed Max");
    parameters_.speedDist.min      = globalParameter_->GetValue<float>(particleName_, "Speed Min");
    parameters_.velocityDistV3.max = globalParameter_->GetValue<Vector3>(particleName_, "SpeedV3 Max");
    parameters_.velocityDistV3.min = globalParameter_->GetValue<Vector3>(particleName_, "SpeedV3 Min");
    parameters_.directionDist.max  = globalParameter_->GetValue<Vector3>(particleName_, "Direction Max");
    parameters_.directionDist.min  = globalParameter_->GetValue<Vector3>(particleName_, "Direction Min");
    parameters_.isFloatVelocity    = globalParameter_->GetValue<bool>(particleName_, "isFloatVelocity");

    // Color
    parameters_.baseColor     = globalParameter_->GetValue<Vector4>(particleName_, "BaseColor");
    parameters_.colorDist.max = globalParameter_->GetValue<Vector4>(particleName_, "Color Max");
    parameters_.colorDist.min = globalParameter_->GetValue<Vector4>(particleName_, "Color Min");

    // その他
    intervalTime_        = globalParameter_->GetValue<float>(particleName_, "IntervalTime");
    parameters_.gravity  = globalParameter_->GetValue<float>(particleName_, "Gravity");
    parameters_.lifeTime = globalParameter_->GetValue<float>(particleName_, "LifeTime");
    particleCount_       = globalParameter_->GetValue<int32_t>(particleName_, "Particle Count");
    maxParticleNum_      = globalParameter_->GetValue<int32_t>(particleName_, "Max Particle Num");

    // Flag
    parameters_.isScalerScale         = globalParameter_->GetValue<bool>(particleName_, "isScalerScale");
    parameters_.isRotateForDirection  = globalParameter_->GetValue<bool>(particleName_, "isRotateforDirection");
    groupParamaters_.isBillBord       = globalParameter_->GetValue<bool>(particleName_, "isBillBord");
    groupParamaters_.adaptRotate_.isX = globalParameter_->GetValue<bool>(particleName_, "AdaptRotateIsX");
    groupParamaters_.adaptRotate_.isY = globalParameter_->GetValue<bool>(particleName_, "AdaptRotateIsY");
    groupParamaters_.adaptRotate_.isZ = globalParameter_->GetValue<bool>(particleName_, "AdaptRotateIsZ");
    isShot_                           = globalParameter_->GetValue<bool>(particleName_, "isShot");
    groupParamaters_.isAlphaNoMove    = globalParameter_->GetValue<bool>(particleName_, "isAlphaNoMove");

    isMoveForRail_ = globalParameter_->GetValue<bool>(particleName_, "isRailRoop");
    isRailRoop_    = globalParameter_->GetValue<bool>(particleName_, "isMoveForRail");
    moveSpeed_     = globalParameter_->GetValue<float>(particleName_, "moveSpeed");

    // EaseParam
    parameters_.scaleEaseParam.baseParam.isEase      = globalParameter_->GetValue<bool>(particleName_, "scaleEaseParm.isScaleEase");
    parameters_.scaleEaseParam.baseParam.maxTime     = globalParameter_->GetValue<float>(particleName_, "scaleEaseParm.maxTime");
    parameters_.scaleEaseParam.baseParam.easeTypeInt = globalParameter_->GetValue<int>(particleName_, "scaleEaseParm.easeTypeInt");
    parameters_.scaleEaseParam.endValueF.max         = globalParameter_->GetValue<float>(particleName_, "scaleEaseParm.endValueF.max");
    parameters_.scaleEaseParam.endValueF.min         = globalParameter_->GetValue<float>(particleName_, "scaleEaseParm.endValueF.min");
    parameters_.scaleEaseParam.endValueV3.max        = globalParameter_->GetValue<Vector3>(particleName_, "scaleEaseParm.endValueV3.max");
    parameters_.scaleEaseParam.endValueV3.min        = globalParameter_->GetValue<Vector3>(particleName_, "scaleEaseParm.endValueV3.min");
    parameters_.scaleEaseParam.baseParam.backRatio   = globalParameter_->GetValue<float>(particleName_, "scaleEaseParm.backRatio");

    // Mode
    billBordType_ = globalParameter_->GetValue<int>(particleName_, "preBillBordType_");
    blendMode_    = globalParameter_->GetValue<int>(particleName_, "blendMode");

    // Texture
    selectedTexturePath_ = globalParameter_->GetValue<std::string>(particleName_, "selectedTexturePath_");

    // Primitive
    useModel_         = globalParameter_->GetValue<bool>(particleName_, "useModel");
    primitiveTypeInt_ = globalParameter_->GetValue<int32_t>(particleName_, "primitiveTypeInt");
    modelFilePath_    = globalParameter_->GetValue<std::string>(particleName_, "modelFilePath");
}

///=================================================================================
/// Editor 更新
///=================================================================================
void ParticleParameter::EditorUpdate() {
#ifdef _DEBUG

    ImGui::Begin(particleName_.c_str());
    ImGui::PushID(particleName_.c_str());

    // Color
    if (ImGui::CollapsingHeader("Color")) {
        ImGui::SeparatorText("Base Color:");
        ImGui::ColorEdit4("Base", &parameters_.baseColor.x);

        ImGui::SeparatorText("Color Range:");
        ImGui::ColorEdit4("Max", &parameters_.colorDist.max.x);
        ImGui::ColorEdit4("Min", &parameters_.colorDist.min.x);
    }

    /// rail
    if (ImGui::CollapsingHeader("MoveForRail")) {
        ImGui::SeparatorText("Paramater");
        ImGui::Checkbox("isRailRoop", &isRailRoop_);
        ImGui::Checkbox("isMoveForRail", &isMoveForRail_);
        ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.1f);

        ImGui::SeparatorText("ControlPoints:");
        railManager_->ImGuiEdit();
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

    // scale Parm
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

    // UV Scroll
    if (ImGui::CollapsingHeader("UV Parameters")) {
        ImGui::SeparatorText("UV Position:");
        ImGui::DragFloat2("UV_Pos", &parameters_.uvParam.pos.x, 0.01f);

        ImGui::SeparatorText("UV Rotation:");
        ImGui::DragFloat3("UV_Rotate", &parameters_.uvParam.rotate.x, 0.1f);

        ImGui::SeparatorText("UV Animation:");
        ImGui::InputInt("Num of Frames", &parameters_.uvParam.numOfFrame);
        ImGui::DragFloat("Scrool Speed", &parameters_.uvParam.frameScrollSpeed, 0.01f);
        ImGui::Checkbox("Is Roop", &parameters_.uvParam.isLoop);
        ImGui::Checkbox("Is ScroolEachPixel", &parameters_.uvParam.isScrollEachPixel);
        ImGui::Checkbox("Is Scrool", &parameters_.uvParam.isScroll);
        ImGui::Checkbox("Is IsFlipX", &parameters_.uvParam.isFlipX);
        ImGui::Checkbox("Is IsFlipY", &parameters_.uvParam.isFlipY);
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

        const char* billBordItems[] = {"XYZ", "X", "Y", "Z"}; // ビルボードの種類

        if (ImGui::Combo("Billboard Type", &billBordType_, billBordItems, IM_ARRAYSIZE(billBordItems))) {

            groupParamaters_.billBordType = static_cast<BillboardType>(billBordType_);
        }
    }

    /// blend mode
    if (ImGui::CollapsingHeader("BlendMode")) {

        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"}; // BlendMode
        // ビルボードの種類を選択するコンボボックス
        if (ImGui::Combo("Blend Mode", &blendMode_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            // 選択した値を反映
            groupParamaters_.blendMode = static_cast<BlendMode>(blendMode_);
        }
    }

    // frag setting
    if (ImGui::CollapsingHeader("Frag")) {

        // IsRotateforDirection のチェックボックス
        ImGui::Checkbox("IsRotateforDirection", &parameters_.isRotateForDirection);
        ImGui::Checkbox("IsShot", &isShot_);
        ImGui::Checkbox("isAlphaNoMove", &groupParamaters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    // パーティクル切り替え
    ParticleChange();

    globalParameter_->ParamSaveForImGui(particleName_, folderPath_);
    globalParameter_->ParamLoadForImGui(particleName_, folderPath_);

    ImGui::PopID();
    ImGui::End();
#endif // _DEBUG
}

void ParticleParameter::ScaleParmEditor() {
    // Scale
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");

        ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);
        ImGui::Checkbox("IsEasingMode", &parameters_.scaleEaseParam.baseParam.isEase);

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

// ImGuiファイル項目選択
void ParticleParameter::DisplayFileSelection(const std::string& header, const std::vector<std::string>& filenames, int& selectedIndex, const std::function<void(const std::string&)>& onApply) {
    if (!filenames.empty()) {
        std::vector<const char*> names;
        for (const auto& file : filenames) {
            names.push_back(file.c_str());
        }

        if (ImGui::CollapsingHeader(header.c_str())) {
            // リストボックスの表示
            ImGui::ListBox(header.c_str(), &selectedIndex, names.data(), static_cast<int>(names.size()));

            // 適用ボタン
            if (ImGui::Button(("Apply:" + header).c_str())) {
                onApply(filenames[selectedIndex]);
            }
        }
    } else {
        ImGui::Text("No %s files found.", header.c_str());
    }
}

// Particleファイル選択
void ParticleParameter::ParticleChange() {
    static int selectedIndex           = 0; // 現在選択中のインデックス
    std::vector<std::string> filenames = ParticleManager::GetInstance()->GetParticleFiles();

    DisplayFileSelection("SelectParticle", filenames, selectedIndex, [this](const std::string& selectedFile) {
        globalParameter_->CopyGroup(selectedFile, particleName_);
        globalParameter_->SyncParamForGroup(particleName_);

        ImGui::Text("Load Successful: %s", (folderPath_ + selectedFile).c_str());
    });
}

/// texture選択
void ParticleParameter::ImGuiTextureSelection() {
    static int selectedIndex           = 0; // 現在選択中のインデックス
    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    DisplayFileSelection("SelectTexture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);

        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}

void ParticleParameter::ApplyTexture(const std::string& textureName) {
    // テクスチャ
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
    ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}

void ParticleParameter::InitAdaptTexture() {
    if (selectedTexturePath_ == "") {
        return;
    }
    // テクスチャのハンドルを取得
    ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}