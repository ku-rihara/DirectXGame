// ParticleParameter.cpp
#include "ParticleParameter.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "ParticleManager.h"
#include "Function/GetFile.h"
#include <format>
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
    parameters_.isRotateforDirection = false;
    particleCount_                   = 0;
    intervalTime_                    = 1.0f;
    groupParamaters_.isBillBord      = true;
    parameters_.uvParm.numOfFrame    = 1;
    selectedTexturePath_             = "resources/Texture/uvChecker.png";
}

void ParticleParameter::Init() {

    ParameterInit();

   ///* グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(particleName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(particleName_);

    // Editor上で設定したテクスチャを適応
    AdaptTexture();

    //rail適応
    

    /// セッティングしたもの適応
    groupParamaters_.billBordType      = static_cast<BillboardType>(billBordType_);
    groupParamaters_.isShot            = isShot_;
    groupParamaters_.blendMode         = static_cast<BlendMode>(blendMode_);
    parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
}


void ParticleParameter::BindParams() {
    // Position
    globalParameter_->Bind(particleName_, "Position Base", &parameters_.emitPos);
    globalParameter_->Bind(particleName_, "Position Max", &parameters_.positionDist.max);
    globalParameter_->Bind(particleName_, "Position Min", &parameters_.positionDist.min);

    // Scale
    globalParameter_->Bind(particleName_, "Scale Max", &parameters_.scaleDist.max);
    globalParameter_->Bind(particleName_, "Scale Min", &parameters_.scaleDist.min);
    globalParameter_->Bind(particleName_, "ScaleV3 Max", &parameters_.scaleDistV3.max);
    globalParameter_->Bind(particleName_, "ScaleV3 Min", &parameters_.scaleDistV3.min);

    // Rotate
    globalParameter_->Bind(particleName_, "Rotate Base", &parameters_.baseRotate);
    globalParameter_->Bind(particleName_, "Rotate Max", &parameters_.rotateDist.max);
    globalParameter_->Bind(particleName_, "Rotate Min", &parameters_.rotateDist.min);
    globalParameter_->Bind(particleName_, "RotateSpeed Max", &parameters_.rotateSpeedDist.max);
    globalParameter_->Bind(particleName_, "RotateSpeed Min", &parameters_.rotateSpeedDist.min);

    // UV
    globalParameter_->Bind(particleName_, "UV Pos", &parameters_.uvParm.pos);
    globalParameter_->Bind(particleName_, "UV Rotate", &parameters_.uvParm.rotate);
    globalParameter_->Bind(particleName_, "UV NumOfFrame", &parameters_.uvParm.numOfFrame);
    globalParameter_->Bind(particleName_, "UV ScroolSpeed", &parameters_.uvParm.frameScroolSpeed);
    globalParameter_->Bind(particleName_, "UV IsRoop", &parameters_.uvParm.isRoop);
    globalParameter_->Bind(particleName_, "UV isScroolEachPixel", &parameters_.uvParm.isScroolEachPixel);
    globalParameter_->Bind(particleName_, "UV isScrool", &parameters_.uvParm.isScrool);
    globalParameter_->Bind(particleName_, "UV isFlipX", &parameters_.uvParm.isFlipX);
    globalParameter_->Bind(particleName_, "UV isFlipY", &parameters_.uvParm.isFlipY);

    // Velocity
    globalParameter_->Bind(particleName_, "Speed Max", &parameters_.speedDist.max);
    globalParameter_->Bind(particleName_, "Speed Min", &parameters_.speedDist.min);
    globalParameter_->Bind(particleName_, "SpeedV3 Max", &parameters_.velocityDistV3.max);
    globalParameter_->Bind(particleName_, "SpeedV3 Min", &parameters_.velocityDistV3.min);
    globalParameter_->Bind(particleName_, "Direction Max", &parameters_.directionDist.max);
    globalParameter_->Bind(particleName_, "Direction Min", &parameters_.directionDist.min);
    globalParameter_->Bind(particleName_, "isFloatVelocity", &parameters_.isFloatVelocity);

    // Color
    globalParameter_->Bind(particleName_, "BaseColor", &parameters_.baseColor);
    globalParameter_->Bind(particleName_, "Color Max", &parameters_.colorDist.max);
    globalParameter_->Bind(particleName_, "Color Min", &parameters_.colorDist.min);

    // その他
    globalParameter_->Bind(particleName_, "IntervalTime", &intervalTime_);
    globalParameter_->Bind(particleName_, "Gravity", &parameters_.gravity);
    globalParameter_->Bind(particleName_, "LifeTime", &parameters_.lifeTime);
    globalParameter_->Bind(particleName_, "Particle Count", &particleCount_);

    /// frag
    globalParameter_->Bind(particleName_, "isScalerScale", &parameters_.isScalerScale);
    globalParameter_->Bind(particleName_, "isRotateforDirection", &parameters_.isRotateforDirection);
    globalParameter_->Bind(particleName_, "isBillBord", &groupParamaters_.isBillBord);
    globalParameter_->Bind(particleName_, "AdaptRotateIsX", &groupParamaters_.adaptRotate_.isX);
    globalParameter_->Bind(particleName_, "AdaptRotateIsY", &groupParamaters_.adaptRotate_.isY);
    globalParameter_->Bind(particleName_, "AdaptRotateIsZ", &groupParamaters_.adaptRotate_.isZ);
    globalParameter_->Bind(particleName_, "isShot", &isShot_);
    globalParameter_->Bind(particleName_, "isAlphaNoMove", &groupParamaters_.isAlphaNoMove);

     globalParameter_->Bind(particleName_, "isMoveForRail", &isMoveForRail_);
    globalParameter_->Bind(particleName_, "moveSpeed", &moveSpeed_);


    // easeParm
    globalParameter_->Bind(particleName_, "scaleEaseParm.isScaleEase", &parameters_.scaleEaseParm.isScaleEase);
    globalParameter_->Bind(particleName_, "scaleEaseParm.maxTime", &parameters_.scaleEaseParm.maxTime);
    globalParameter_->Bind(particleName_, "scaleEaseParm.easeTypeInt", &parameters_.scaleEaseParm.easeTypeInt);
    globalParameter_->Bind(particleName_, "scaleEaseParm.endValueF.max", &parameters_.scaleEaseParm.endValueF.max);
    globalParameter_->Bind(particleName_, "scaleEaseParm.endValueF.min", &parameters_.scaleEaseParm.endValueF.min);
    globalParameter_->Bind(particleName_, "scaleEaseParm.endValueV3.max", &parameters_.scaleEaseParm.endValueV3.max);
    globalParameter_->Bind(particleName_, "scaleEaseParm.endValueV3.min", &parameters_.scaleEaseParm.endValueV3.min);

    // mpde Setting
    globalParameter_->Bind(particleName_, "preBillBordType_", &billBordType_);
    globalParameter_->Bind(particleName_, "blendMode", &blendMode_);

    // テクスチャ
    globalParameter_->Bind(particleName_, "selectedTexturePath_", &selectedTexturePath_);
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
        ImGui::Checkbox("IsRotateforDirection", &parameters_.isRotateforDirection);
        ImGui::Checkbox("IsShot", &isShot_);
        ImGui::Checkbox("isAlphaNoMove", &groupParamaters_.isAlphaNoMove);
    }

    ImGuiTextureSelection();

    // パーティクル切り替え
    ParticleChange();

    globalParameter_->ParamSaveForImGui(particleName_,folderName_);
    globalParameter_->ParamLoadForImGui(particleName_, folderName_);

    ImGui::PopID();
    ImGui::End();
#endif // _DEBUG
}
void ParticleParameter::ScaleParmEditor() {
    // Scale
    if (ImGui::CollapsingHeader("Scale")) {
        ImGui::SeparatorText("Scale Mode");

        ImGui::Checkbox("IsScalerScale", &parameters_.isScalerScale);
        ImGui::Checkbox("IsEasingMode", &parameters_.scaleEaseParm.isScaleEase);

        if (parameters_.isScalerScale) {
            ImGui::SeparatorText("Scaler Range");
            ImGui::DragFloat("Scale Max", &parameters_.scaleDist.max, 0.1f);
            ImGui::DragFloat("Scale Min", &parameters_.scaleDist.min, 0.1f);
        } else {
            ImGui::SeparatorText("V3 Range");
            ImGui::DragFloat3("ScaleV3 Max", reinterpret_cast<float*>(&parameters_.scaleDistV3.max), 0.1f);
            ImGui::DragFloat3("ScaleV3 Min", reinterpret_cast<float*>(&parameters_.scaleDistV3.min), 0.1f);
        }

        // EaseParm
        if (parameters_.scaleEaseParm.isScaleEase) {

            if (parameters_.isScalerScale) {
                ImGui::SeparatorText("EaseRange Float");
                ImGui::DragFloat("EaseValue Min", &parameters_.scaleEaseParm.endValueF.min, 0.1f);
                ImGui::DragFloat("EaseValue Max", &parameters_.scaleEaseParm.endValueF.max, 0.1f);
            } else {
                ImGui::SeparatorText("EaseRange V3");
                ImGui::DragFloat3("EaseValueV3 Max", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.max), 0.1f);
                ImGui::DragFloat3("EaseValueV3 Min", reinterpret_cast<float*>(&parameters_.scaleEaseParm.endValueV3.min), 0.1f);
            }
            // イージングパラメータ
            ImGui::SeparatorText("Ease Paramater");
            ImGui::DragFloat("maxTime", &parameters_.scaleEaseParm.maxTime, 0.01f);
            ImGui::SeparatorText("EaseType");
            // イージング種類
            const char* easeItems[] = {"InSine", "OutSine", "OutBack", "OutQuint"};

            if (ImGui::Combo("Easing Type", &parameters_.scaleEaseParm.easeTypeInt, easeItems, IM_ARRAYSIZE(easeItems))) {

                parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
            }
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
        globalParameter_->SyncParamForGroup(selectedFile);
        globalParameter_->LoadFile(selectedFile, folderName_);
      /*  ApplyGlobalParameter(selectedFile);
        globalParameter_->LoadFile(selectedFile, folderName_);*/
        ImGui::Text("Load Successful: %s", (folderName_ + selectedFile).c_str());
    });
}

/// texture選択
void ParticleParameter::ImGuiTextureSelection() {
    static int selectedIndex           = 0; // 現在選択中のインデックス
    std::vector<std::string> filenames = GetFileNamesForDyrectry(textureFilePath_);

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



void ParticleParameter::AdaptTexture() {
    if (selectedTexturePath_ == "") {
        return;
    }
    // テクスチャのハンドルを取得
    ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}