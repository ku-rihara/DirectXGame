// ParticleParameter.cpp
#include "ParticleParameter.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "ParticleManager.h"
#include <format>
#include <imgui.h>

void ParticleParameter::ParameterInit() {

    parameters_.targetPos            = {0.0f, 0.0f, 0.0f};
    parameters_.lifeTime             = 0.0f;
    parameters_.gravity              = 0.0f;
    parameters_.baseColor            = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.colorDist.min        = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.colorDist.max        = {0.0f, 0.0f, 0.0f, 0.0f};
    parameters_.isScalerScale        = true;
    parameters_.isRotateforDirection = false;
    particleCount_                   = 0;
    intervalTime_                    = 1.0f;
    groupParamaters_.isBillBord      = true;
    selectedTexturePath_             = "";
}

void ParticleParameter::Init() {

    ParameterInit();

    /// グローバルパラメータ作成
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(particleName_, false);

    // 　グループ追加
    AddParmGroup();
    ApplyGlobalParameter(particleName_);

    // Editor上で設定したテクスチャを適応
    AdaptTexture();

    /// セッティングしたもの適応
    groupParamaters_.billBordType      = static_cast<BillboardType>(billBordType_);
    groupParamaters_.isShot            = isShot_;
    groupParamaters_.blendMode         = static_cast<BlendMode>(blendMode_);
    parameters_.scaleEaseParm.easeType = static_cast<EaseType>(parameters_.scaleEaseParm.easeTypeInt);
}

void ParticleParameter::ParmLoadForImGui() {
    // ロードボタン
    if (ImGui::Button(std::format("Load {}", particleName_).c_str())) {
        globalParameter_->LoadFile(particleName_, folderName_);
        // セーブ完了メッセージ
        ImGui::Text("Load Successful: %s", (folderName_ + particleName_).c_str());
        ApplyGlobalParameter(particleName_);
    }
}

void ParticleParameter::ParmSaveForImGui() {
    // 保存ボタン
    if (ImGui::Button(std::format("Save {}", particleName_).c_str())) {
        globalParameter_->SaveFile(particleName_, folderName_);
        // セーブ完了メッセージ
        std::string message = std::format("{}.json saved.", folderName_ + particleName_);
        MessageBoxA(nullptr, message.c_str(), "GlobalParameter", 0);
    }
}

void ParticleParameter::AddParmGroup() {
    // Position
    globalParameter_->AddItem(particleName_, "Position Base", parameters_.emitPos);
    globalParameter_->AddItem(particleName_, "Position Max", parameters_.positionDist.max);
    globalParameter_->AddItem(particleName_, "Position Min", parameters_.positionDist.min);

    // Scale
    globalParameter_->AddItem(particleName_, "Scale Max", parameters_.scaleDist.max);
    globalParameter_->AddItem(particleName_, "Scale Min", parameters_.scaleDist.min);
    globalParameter_->AddItem(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max);
    globalParameter_->AddItem(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min);

    // Rotate
    globalParameter_->AddItem(particleName_, "Rotate Base", parameters_.baseRotate);
    globalParameter_->AddItem(particleName_, "Rotate Max", parameters_.rotateDist.max);
    globalParameter_->AddItem(particleName_, "Rotate Min", parameters_.rotateDist.min);
    globalParameter_->AddItem(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
    globalParameter_->AddItem(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

    // UV
    globalParameter_->AddItem(particleName_, "UV Pos", parameters_.uvParm.pos);
    globalParameter_->AddItem(particleName_, "UV Rotate", parameters_.uvParm.rotate);
    globalParameter_->AddItem(particleName_, "UV NumOfFrame", parameters_.uvParm.numOfFrame);
    globalParameter_->AddItem(particleName_, "UV ScroolSpeed", parameters_.uvParm.frameScroolSpeed);
    globalParameter_->AddItem(particleName_, "UV IsRoop", parameters_.uvParm.isRoop);
    globalParameter_->AddItem(particleName_, "UV isScroolEachPixel", parameters_.uvParm.isScroolEachPixel);
    globalParameter_->AddItem(particleName_, "UV isScrool", parameters_.uvParm.isScrool);

    // Velocity
    globalParameter_->AddItem(particleName_, "Speed Max", parameters_.speedDist.max);
    globalParameter_->AddItem(particleName_, "Speed Min", parameters_.speedDist.min);
    globalParameter_->AddItem(particleName_, "Direction Max", parameters_.directionDist.max);
    globalParameter_->AddItem(particleName_, "Direction Min", parameters_.directionDist.min);

    // Color
    globalParameter_->AddItem(particleName_, "BaseColor", parameters_.baseColor);
    globalParameter_->AddItem(particleName_, "Color Max", parameters_.colorDist.max);
    globalParameter_->AddItem(particleName_, "Color Min", parameters_.colorDist.min);

    // その他
    globalParameter_->AddItem(particleName_, "IntervalTime", intervalTime_);
    globalParameter_->AddItem(particleName_, "Gravity", parameters_.gravity);
    globalParameter_->AddItem(particleName_, "LifeTime", parameters_.lifeTime);
    globalParameter_->AddItem(particleName_, "Particle Count", particleCount_);

    /// frag
    globalParameter_->AddItem(particleName_, "isScalerScale", parameters_.isScalerScale);
    globalParameter_->AddItem(particleName_, "isRotateforDirection", parameters_.isRotateforDirection);
    globalParameter_->AddItem(particleName_, "isBillBord", groupParamaters_.isBillBord);
    globalParameter_->AddItem(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX);
    globalParameter_->AddItem(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY);
    globalParameter_->AddItem(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ);
    globalParameter_->AddItem(particleName_, "isShot", isShot_);
    globalParameter_->AddItem(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove);

    // easeParm
    globalParameter_->AddItem(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max);
    globalParameter_->AddItem(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min);

    // mpde Setting
    globalParameter_->AddItem(particleName_, "preBillBordType_", billBordType_);
    globalParameter_->AddItem(particleName_, "blendMode", blendMode_);

    // テクスチャ
    globalParameter_->AddItem(particleName_, "selectedTexturePath_", selectedTexturePath_);
}

void ParticleParameter::SetValues() {
    // Position
    globalParameter_->SetValue(particleName_, "Position Base", parameters_.emitPos);
    globalParameter_->SetValue(particleName_, "Position Max", parameters_.positionDist.max);
    globalParameter_->SetValue(particleName_, "Position Min", parameters_.positionDist.min);

    // Scale
    globalParameter_->SetValue(particleName_, "Scale Max", parameters_.scaleDist.max);
    globalParameter_->SetValue(particleName_, "Scale Min", parameters_.scaleDist.min);
    globalParameter_->SetValue(particleName_, "ScaleV3 Max", parameters_.scaleDistV3.max);
    globalParameter_->SetValue(particleName_, "ScaleV3 Min", parameters_.scaleDistV3.min);

    // Rotate
    globalParameter_->SetValue(particleName_, "Rotate Base", parameters_.baseRotate);
    globalParameter_->SetValue(particleName_, "Rotate Max", parameters_.rotateDist.max);
    globalParameter_->SetValue(particleName_, "Rotate Min", parameters_.rotateDist.min);
    globalParameter_->SetValue(particleName_, "RotateSpeed Max", parameters_.rotateSpeedDist.max);
    globalParameter_->SetValue(particleName_, "RotateSpeed Min", parameters_.rotateSpeedDist.min);

    // UV
    globalParameter_->SetValue(particleName_, "UV Pos", parameters_.uvParm.pos);
    globalParameter_->SetValue(particleName_, "UV Rotate", parameters_.uvParm.rotate);
    globalParameter_->SetValue(particleName_, "UV NumOfFrame", parameters_.uvParm.numOfFrame);
    globalParameter_->SetValue(particleName_, "UV ScroolSpeed", parameters_.uvParm.frameScroolSpeed);
    globalParameter_->SetValue(particleName_, "UV IsRoop", parameters_.uvParm.isRoop);
    globalParameter_->SetValue(particleName_, "UV isScroolEachPixel", parameters_.uvParm.isScroolEachPixel);
    globalParameter_->SetValue(particleName_, "UV isScrool", parameters_.uvParm.isScrool);

    // Velocity
    globalParameter_->SetValue(particleName_, "Speed Max", parameters_.speedDist.max);
    globalParameter_->SetValue(particleName_, "Speed Min", parameters_.speedDist.min);
    globalParameter_->SetValue(particleName_, "Direction Max", parameters_.directionDist.max);
    globalParameter_->SetValue(particleName_, "Direction Min", parameters_.directionDist.min);

    // Color
    globalParameter_->SetValue(particleName_, "BaseColor", parameters_.baseColor);
    globalParameter_->SetValue(particleName_, "Color Max", parameters_.colorDist.max);
    globalParameter_->SetValue(particleName_, "Color Min", parameters_.colorDist.min);

    // その他
    globalParameter_->SetValue(particleName_, "IntervalTime", intervalTime_);
    globalParameter_->SetValue(particleName_, "Gravity", parameters_.gravity);
    globalParameter_->SetValue(particleName_, "LifeTime", parameters_.lifeTime);
    globalParameter_->SetValue(particleName_, "Particle Count", particleCount_);
    /// frag
    globalParameter_->SetValue(particleName_, "isScalerScale", parameters_.isScalerScale);
    globalParameter_->SetValue(particleName_, "isRotateforDirection", parameters_.isRotateforDirection);
    globalParameter_->SetValue(particleName_, "isBillBord", groupParamaters_.isBillBord);
    globalParameter_->SetValue(particleName_, "AdaptRotateIsX", groupParamaters_.adaptRotate_.isX);
    globalParameter_->SetValue(particleName_, "AdaptRotateIsY", groupParamaters_.adaptRotate_.isY);
    globalParameter_->SetValue(particleName_, "AdaptRotateIsZ", groupParamaters_.adaptRotate_.isZ);
    globalParameter_->SetValue(particleName_, "isShot", isShot_);
    globalParameter_->SetValue(particleName_, "isAlphaNoMove", groupParamaters_.isAlphaNoMove);

    // easeParm
    globalParameter_->SetValue(particleName_, "scaleEaseParm.isScaleEase", parameters_.scaleEaseParm.isScaleEase);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.maxTime", parameters_.scaleEaseParm.maxTime);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.easeTypeInt", parameters_.scaleEaseParm.easeTypeInt);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.max", parameters_.scaleEaseParm.endValueF.max);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueF.min", parameters_.scaleEaseParm.endValueF.min);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.max", parameters_.scaleEaseParm.endValueV3.max);
    globalParameter_->SetValue(particleName_, "scaleEaseParm.endValueV3.min", parameters_.scaleEaseParm.endValueV3.min);

    //
    globalParameter_->SetValue(particleName_, "preBillBordType_", billBordType_);
    globalParameter_->SetValue(particleName_, "blendMode", blendMode_);

    // テクスチャ
    globalParameter_->SetValue(particleName_, "selectedTexturePath_", selectedTexturePath_, GlobalParameter::WidgetType::NONE);
}

void ParticleParameter::ApplyGlobalParameter(const std::string& particleName) {
    // Position
    parameters_.emitPos          = globalParameter_->GetValue<Vector3>(particleName, "Position Base");
    parameters_.positionDist.min = globalParameter_->GetValue<Vector3>(particleName, "Position Min");
    parameters_.positionDist.max = globalParameter_->GetValue<Vector3>(particleName, "Position Max");

    // Scale
    parameters_.scaleDist.min   = globalParameter_->GetValue<float>(particleName, "Scale Min");
    parameters_.scaleDist.max   = globalParameter_->GetValue<float>(particleName, "Scale Max");
    parameters_.scaleDistV3.min = globalParameter_->GetValue<Vector3>(particleName, "ScaleV3 Min");
    parameters_.scaleDistV3.max = globalParameter_->GetValue<Vector3>(particleName, "ScaleV3 Max");

    // Rotate
    parameters_.baseRotate     = globalParameter_->GetValue<Vector3>(particleName, "Rotate Base");
    parameters_.rotateDist.min = globalParameter_->GetValue<Vector3>(particleName, "Rotate Min");
    parameters_.rotateDist.max = globalParameter_->GetValue<Vector3>(particleName, "Rotate Max");

    /// UV
    parameters_.uvParm.pos               = globalParameter_->GetValue<Vector2>(particleName_, "UV Pos");
    parameters_.uvParm.rotate            = globalParameter_->GetValue<Vector3>(particleName_, "UV Rotate");
    parameters_.uvParm.numOfFrame        = globalParameter_->GetValue<int32_t>(particleName_, "UV NumOfFrame");
    parameters_.uvParm.frameScroolSpeed  = globalParameter_->GetValue<float>(particleName_, "UV ScroolSpeed");
    parameters_.uvParm.isRoop            = globalParameter_->GetValue<bool>(particleName_, "UV IsRoop");
    parameters_.uvParm.isScroolEachPixel = globalParameter_->GetValue<bool>(particleName_, "UV isScroolEachPixel");
    parameters_.uvParm.isScrool          = globalParameter_->GetValue<bool>(particleName_, "UV isScrool");

    // Rotate Speed
    parameters_.rotateSpeedDist.min = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Min");
    parameters_.rotateSpeedDist.max = globalParameter_->GetValue<Vector3>(particleName, "RotateSpeed Max");

    // Velocity
    parameters_.speedDist.min     = globalParameter_->GetValue<float>(particleName, "Speed Min");
    parameters_.speedDist.max     = globalParameter_->GetValue<float>(particleName, "Speed Max");
    parameters_.directionDist.min = globalParameter_->GetValue<Vector3>(particleName, "Direction Min");
    parameters_.directionDist.max = globalParameter_->GetValue<Vector3>(particleName, "Direction Max");

    // Color
    parameters_.baseColor     = globalParameter_->GetValue<Vector4>(particleName, "BaseColor");
    parameters_.colorDist.min = globalParameter_->GetValue<Vector4>(particleName, "Color Min");
    parameters_.colorDist.max = globalParameter_->GetValue<Vector4>(particleName, "Color Max");

    // その他
    intervalTime_        = globalParameter_->GetValue<float>(particleName, "IntervalTime");
    parameters_.gravity  = globalParameter_->GetValue<float>(particleName, "Gravity");
    parameters_.lifeTime = globalParameter_->GetValue<float>(particleName, "LifeTime");
    particleCount_       = globalParameter_->GetValue<int32_t>(particleName, "Particle Count");

    /// frag
    parameters_.isScalerScale         = globalParameter_->GetValue<bool>(particleName, "isScalerScale");
    parameters_.isRotateforDirection  = globalParameter_->GetValue<bool>(particleName, "isRotateforDirection");
    groupParamaters_.isBillBord       = globalParameter_->GetValue<bool>(particleName, "isBillBord");
    groupParamaters_.adaptRotate_.isX = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsX");
    groupParamaters_.adaptRotate_.isY = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsY");
    groupParamaters_.adaptRotate_.isZ = globalParameter_->GetValue<bool>(particleName, "AdaptRotateIsZ");
    isShot_                           = globalParameter_->GetValue<bool>(particleName, "isShot");
    groupParamaters_.isAlphaNoMove    = globalParameter_->GetValue<bool>(particleName, "isAlphaNoMove");

    /// easeParm
    parameters_.scaleEaseParm.isScaleEase    = globalParameter_->GetValue<bool>(particleName, "scaleEaseParm.isScaleEase");
    parameters_.scaleEaseParm.maxTime        = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.maxTime");
    parameters_.scaleEaseParm.easeTypeInt    = globalParameter_->GetValue<int32_t>(particleName, "scaleEaseParm.easeTypeInt");
    parameters_.scaleEaseParm.endValueF.max  = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.max");
    parameters_.scaleEaseParm.endValueF.min  = globalParameter_->GetValue<float>(particleName, "scaleEaseParm.endValueF.min");
    parameters_.scaleEaseParm.endValueV3.max = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.max");
    parameters_.scaleEaseParm.endValueV3.min = globalParameter_->GetValue<Vector3>(particleName, "scaleEaseParm.endValueV3.min");

    /// mode setting
    billBordType_ = globalParameter_->GetValue<int32_t>(particleName, "preBillBordType_");
    blendMode_    = globalParameter_->GetValue<int32_t>(particleName, "blendMode");

    // texture
    selectedTexturePath_ = globalParameter_->GetValue<std::string>(particleName, "selectedTexturePath_");
}

void ParticleParameter::AdaptTexture() {
    if (selectedTexturePath_ == "")
        return;
    // テクスチャのハンドルを取得
    ParticleManager::GetInstance()->SetTextureHandle(particleName_, TextureManager::GetInstance()->LoadTexture(selectedTexturePath_));
}