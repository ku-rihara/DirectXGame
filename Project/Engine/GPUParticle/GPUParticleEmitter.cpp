#include "GPUParticleEmitter.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "Function/GetFile.h"
#include "GPUParticleManager.h"
#include "Primitive/IPrimitive.h"
#include <imgui.h>

///=================================================================================
/// パーティクル作成
///=================================================================================
GPUParticleEmitter* GPUParticleEmitter::CreateParticle(
    const std::string& name, const std::string& modelFilePath, int32_t maxCount) {

    auto emitter = std::make_unique<GPUParticleEmitter>();
    emitter->InitWithModel(name, modelFilePath, maxCount);
    emitter->Init();
    return emitter.release();
}

///=================================================================================
/// パーティクル作成(Primitive)
///=================================================================================
GPUParticleEmitter* GPUParticleEmitter::CreateParticlePrimitive(
    const std::string& name, const PrimitiveType& type, int32_t maxCount) {

    auto emitter = std::make_unique<GPUParticleEmitter>();
    emitter->InitWithPrimitive(name, type, maxCount);
    emitter->Init();
    return emitter.release();
}

void GPUParticleEmitter::InitWithModel(
    const std::string& name,
    const std::string& modelFilePath,
    int32_t maxCount) {

    name_ = name;
    GPUParticleManager::GetInstance()->CreateParticleGroup(name_, modelFilePath, maxCount);
}

void GPUParticleEmitter::InitWithPrimitive(
    const std::string& name,
    const PrimitiveType& type,
    int32_t maxCount) {

    name_ = name;
    GPUParticleManager::GetInstance()->CreatePrimitiveParticle(name_, type, maxCount);
}

///=================================================================================
/// 初期化
///=================================================================================
void GPUParticleEmitter::Init() {
    currentTime_ = 0.0f;
    shouldEmit_  = false;

    ParameterInit();

    // GlobalParameter初期化
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(folderPath_ + "/" + name_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(folderPath_ + "/" + name_);

    // Rail初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->Init(name_ + "Emit");

    // デバッグライン
    debugLine_.reset(Line3D::Create(24));
    emitBoxTransform_.Init();

    // テクスチャ適用
    AdaptTexture();

    // ブレンドモード適用
    groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);

    // パラメータ適用
    ApplyParameters();
}

void GPUParticleEmitter::ParameterInit() {
    emitParams_.scaleMin         = {0.5f, 0.5f, 0.5f};
    emitParams_.scaleMax         = {1.5f, 1.5f, 1.5f};
    emitParams_.rotationMin      = {0.0f, 0.0f, 0.0f};
    emitParams_.rotationMax      = {360.0f, 360.0f, 360.0f};
    emitParams_.rotationSpeedMin = {0.0f, 0.0f, 0.0f};
    emitParams_.rotationSpeedMax = {180.0f, 180.0f, 180.0f};
    emitParams_.translateMin     = {-1.0f, -1.0f, -1.0f};
    emitParams_.translateMax     = {1.0f, 1.0f, 1.0f};
    emitParams_.velocityMin      = {-1.0f, -1.0f, -1.0f};
    emitParams_.velocityMax      = {1.0f, 1.0f, 1.0f};
    emitParams_.colorMin         = {1.0f, 1.0f, 1.0f, 1.0f};
    emitParams_.colorMax         = {1.0f, 1.0f, 1.0f, 1.0f};
    emitParams_.lifeTimeMin      = 1.0f;
    emitParams_.lifeTimeMax      = 3.0f;

    emitterSettings_.position  = {0.0f, 0.0f, 0.0f};
    emitterSettings_.radius    = 1.0f;
    emitterSettings_.count     = 10;
    emitterSettings_.frequency = 1.0f;

    selectedTexturePath_ = "Resources/Texture/circle.png";
}

void GPUParticleEmitter::RegisterParams() {
    std::string groupName = folderPath_ + "/" + name_;

    globalParameter_->Regist(groupName, "Scale Min", &emitParams_.scaleMin);
    globalParameter_->Regist(groupName, "Scale Max", &emitParams_.scaleMax);
    globalParameter_->Regist(groupName, "Rotation Min", &emitParams_.rotationMin);
    globalParameter_->Regist(groupName, "Rotation Max", &emitParams_.rotationMax);
    globalParameter_->Regist(groupName, "Rotation Speed Min", &emitParams_.rotationSpeedMin);
    globalParameter_->Regist(groupName, "Rotation Speed Max", &emitParams_.rotationSpeedMax);
    globalParameter_->Regist(groupName, "Translate Min", &emitParams_.translateMin);
    globalParameter_->Regist(groupName, "Translate Max", &emitParams_.translateMax);
    globalParameter_->Regist(groupName, "Velocity Min", &emitParams_.velocityMin);
    globalParameter_->Regist(groupName, "Velocity Max", &emitParams_.velocityMax);
    globalParameter_->Regist(groupName, "Color Min", &emitParams_.colorMin);
    globalParameter_->Regist(groupName, "Color Max", &emitParams_.colorMax);
    globalParameter_->Regist(groupName, "LifeTime Min", &emitParams_.lifeTimeMin);
    globalParameter_->Regist(groupName, "LifeTime Max", &emitParams_.lifeTimeMax);

    globalParameter_->Regist(groupName, "Position", &emitterSettings_.position);
    globalParameter_->Regist(groupName, "Radius", &emitterSettings_.radius);
    globalParameter_->Regist(groupName, "Count", reinterpret_cast<int32_t*>(&emitterSettings_.count));
    globalParameter_->Regist(groupName, "Frequency", &emitterSettings_.frequency);

    globalParameter_->Regist(groupName, "Blend Mode", &blendModeIndex_);
    globalParameter_->Regist(groupName, "Is Active", &groupSettings_.isActive);
    globalParameter_->Regist(groupName, "Texture Path", &selectedTexturePath_);

    globalParameter_->Regist(groupName, "Is Move For Rail", &isMoveForRail_);
    globalParameter_->Regist(groupName, "Is Rail Loop", &isRailLoop_);
    globalParameter_->Regist(groupName, "Move Speed", &moveSpeed_);
}

///=================================================================================
/// 更新
///=================================================================================
void GPUParticleEmitter::Update() {
    if (name_.empty() || !groupSettings_.isActive) {
        return;
    }

    RailMoveUpdate();
    UpdateEmitTransform();
    SetEmitLine();

    // タイミング更新
    currentTime_ += Frame::DeltaTime();

    if (currentTime_ >= emitterSettings_.frequency) {
        shouldEmit_  = true;
        currentTime_ = 0.0f;
    } else {
        shouldEmit_ = false;
    }

    // マネージャーのエミッターデータを更新
    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(name_);
    if (group && group->emitSphereData) {
        ParticleEmit emitterData = *group->emitSphereData;

        if (isMoveForRail_) {
            emitterData.translate = railManager_->GetWorldTransform().GetWorldPos();
        } else {
            emitterData.translate = emitterSettings_.position;
        }

        emitterData.radius        = emitterSettings_.radius;
        emitterData.count         = emitterSettings_.count;
        emitterData.frequency     = emitterSettings_.frequency;
        emitterData.frequencyTime = currentTime_;
        emitterData.emit          = shouldEmit_ ? 1 : 0;

        GPUParticleManager::GetInstance()->SetEmitterSphere(name_, emitterData);
    }
}

void GPUParticleEmitter::RailMoveUpdate() {
    if (!isStartRailMove_) {
        return;
    }
    railManager_->Update(moveSpeed_);

    if (railManager_->GetRailMoveTime() < 1.0f) {
        return;
    }

    if (!isRailLoop_) {
        isStartRailMove_ = false;
    }
}

void GPUParticleEmitter::UpdateEmitTransform() {
    if (isMoveForRail_) {
        emitBoxTransform_.translation_ = railManager_->GetWorldTransform().GetWorldPos();
    } else {
        emitBoxTransform_.translation_ = emitterSettings_.position;
    }

    emitBoxTransform_.scale_ = {
        emitParams_.translateMax.x - emitParams_.translateMin.x,
        emitParams_.translateMax.y - emitParams_.translateMin.y,
        emitParams_.translateMax.z - emitParams_.translateMin.z};

    emitBoxTransform_.UpdateMatrix();
}

void GPUParticleEmitter::SetEmitLine() {
#ifdef _DEBUG
    if (isMoveForRail_) {
        railManager_->SetCubeLine(emitBoxTransform_.scale_);
    } else {
        debugLine_->SetCubeWireframe(
            emitBoxTransform_.GetWorldPos(),
            emitBoxTransform_.scale_,
            Vector4::kWHITE());
    }
#endif
}

///=================================================================================
/// エミット
///=================================================================================
void GPUParticleEmitter::Emit() {
    if (name_.empty() || !shouldEmit_ || !groupSettings_.isActive) {
        return;
    }

    GPUParticleManager::GetInstance()->Emit(name_);
}

void GPUParticleEmitter::StartRailEmit() {
    isStartRailMove_ = true;
    railManager_->SetRailMoveTime(0.0f);
    railManager_->SetIsRoop(isRailLoop_);
}

///=================================================================================
/// パラメータ適用
///=================================================================================
void GPUParticleEmitter::ApplyParameters() {
    EmitParameter shaderParams;

    // Degree to Radian conversion
    const float degToRad     = 3.14159f / 180.0f;
    shaderParams.rotationMin = {
        emitParams_.rotationMin.x * degToRad,
        emitParams_.rotationMin.y * degToRad,
        emitParams_.rotationMin.z * degToRad};
    shaderParams.rotationMax = {
        emitParams_.rotationMax.x * degToRad,
        emitParams_.rotationMax.y * degToRad,
        emitParams_.rotationMax.z * degToRad};
    shaderParams.rotationSpeedMin = {
        emitParams_.rotationSpeedMin.x * degToRad,
        emitParams_.rotationSpeedMin.y * degToRad,
        emitParams_.rotationSpeedMin.z * degToRad};
    shaderParams.rotationSpeedMax = {
        emitParams_.rotationSpeedMax.x * degToRad,
        emitParams_.rotationSpeedMax.y * degToRad,
        emitParams_.rotationSpeedMax.z * degToRad};

    shaderParams.scaleMin     = emitParams_.scaleMin;
    shaderParams.scaleMax     = emitParams_.scaleMax;
    shaderParams.translateMin = emitParams_.translateMin;
    shaderParams.translateMax = emitParams_.translateMax;
    shaderParams.velocityMin  = emitParams_.velocityMin;
    shaderParams.velocityMax  = emitParams_.velocityMax;
    shaderParams.colorMin     = emitParams_.colorMin;
    shaderParams.colorMax     = emitParams_.colorMax;
    shaderParams.lifeTimeMin  = emitParams_.lifeTimeMin;
    shaderParams.lifeTimeMax  = emitParams_.lifeTimeMax;

    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(name_);
    if (group && group->resourceData) {
        group->resourceData->UpdateEmitParamData(shaderParams);
    }
}

///=================================================================================
/// エディタ更新
///=================================================================================
void GPUParticleEmitter::EditorUpdate() {
#ifdef _DEBUG
    ImGui::Begin(("GPU Particle: " + name_).c_str());
    ImGui::PushID(name_.c_str());

    EmitParameterEditor();
    EmitterSettingsEditor();

    if (ImGui::CollapsingHeader("Group Settings")) {
        ImGui::Checkbox("Is Active", &groupSettings_.isActive);

        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("Blend Mode", &blendModeIndex_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);
        }
    }

    if (ImGui::CollapsingHeader("Rail Movement")) {
        ImGui::Checkbox("Is Move For Rail", &isMoveForRail_);
        ImGui::Checkbox("Is Rail Loop", &isRailLoop_);
        ImGui::DragFloat("Move Speed", &moveSpeed_, 0.1f);

        ImGui::SeparatorText("Control Points:");
        railManager_->ImGuiEdit();

        if (ImGui::Button("Start Rail Emit")) {
            StartRailEmit();
        }
    }

    ImGuiTextureSelection();
    ParticlePresetChange();

    if (ImGui::Button("Apply Parameters")) {
        ApplyParameters();
    }

    std::string groupName = folderPath_ + "/" + name_;
    globalParameter_->ParamSaveForImGui(groupName, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName, folderPath_);

    ImGui::PopID();
    ImGui::End();
#endif
}

void GPUParticleEmitter::EmitParameterEditor() {
    if (ImGui::CollapsingHeader("Emit Parameters")) {
        if (ImGui::TreeNode("Scale")) {
            ImGui::DragFloat3("Min", &emitParams_.scaleMin.x, 0.01f);
            ImGui::DragFloat3("Max", &emitParams_.scaleMax.x, 0.01f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation (Degree)")) {
            ImGui::DragFloat3("Min", &emitParams_.rotationMin.x, 1.0f, -360.0f, 360.0f);
            ImGui::DragFloat3("Max", &emitParams_.rotationMax.x, 1.0f, -360.0f, 360.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation Speed (Degree/sec)")) {
            ImGui::DragFloat3("Min", &emitParams_.rotationSpeedMin.x, 1.0f, 0.0f, 720.0f);
            ImGui::DragFloat3("Max", &emitParams_.rotationSpeedMax.x, 1.0f, 0.0f, 720.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Position Offset")) {
            ImGui::DragFloat3("Min", &emitParams_.translateMin.x, 0.1f);
            ImGui::DragFloat3("Max", &emitParams_.translateMax.x, 0.1f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Velocity")) {
            ImGui::DragFloat3("Min", &emitParams_.velocityMin.x, 0.1f);
            ImGui::DragFloat3("Max", &emitParams_.velocityMax.x, 0.1f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Color")) {
            ImGui::ColorEdit4("Min", &emitParams_.colorMin.x);
            ImGui::ColorEdit4("Max", &emitParams_.colorMax.x);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("LifeTime")) {
            ImGui::DragFloat("Min", &emitParams_.lifeTimeMin, 0.1f, 0.1f, 10.0f);
            ImGui::DragFloat("Max", &emitParams_.lifeTimeMax, 0.1f, 0.1f, 10.0f);
            ImGui::TreePop();
        }
    }
}

void GPUParticleEmitter::EmitterSettingsEditor() {
    if (ImGui::CollapsingHeader("Emitter Settings")) {
        ImGui::DragFloat3("Position", &emitterSettings_.position.x, 0.1f);
        ImGui::DragFloat("Radius", &emitterSettings_.radius, 0.1f, 0.0f, 100.0f);
        ImGui::DragInt("Count", reinterpret_cast<int*>(&emitterSettings_.count), 1, 1, 1000);
        ImGui::DragFloat("Frequency", &emitterSettings_.frequency, 0.01f, 0.01f, 10.0f);
    }
}

void GPUParticleEmitter::ImGuiTextureSelection() {
    static int selectedIndex           = 0;
    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    DisplayFileSelection("Select Texture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);
        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}

void GPUParticleEmitter::DisplayFileSelection(
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

            if (ImGui::Button(("Apply: " + header).c_str())) {
                onApply(filenames[selectedIndex]);
            }
        }
    } else {
        ImGui::Text("No %s files found.", header.c_str());
    }
}

void GPUParticleEmitter::ParticlePresetChange() {
    static int selectedIndex           = 0;
    std::vector<std::string> filenames = GetFileNamesForDirectory(GlobalParameter::GetInstance()->GetDirectoryPath() + folderPath_);

    DisplayFileSelection("Select Preset", filenames, selectedIndex, [this](const std::string& selectedFile) {
        std::string groupName = folderPath_ + "/" + name_;
        globalParameter_->CopyGroup(folderPath_ + "/" + selectedFile, groupName);
        globalParameter_->SyncParamForGroup(groupName);
        ApplyParameters();
        ImGui::Text("Preset Loaded: %s", selectedFile.c_str());
    });
}

void GPUParticleEmitter::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
    uint32_t handle      = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(name_, handle);
}

void GPUParticleEmitter::AdaptTexture() {
    if (selectedTexturePath_.empty()) {
        return;
    }
    uint32_t handle = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(name_, handle);
}

void GPUParticleEmitter::SetTexture(uint32_t textureHandle) {
    GPUParticleManager::GetInstance()->SetTextureHandle(name_, textureHandle);
}

void GPUParticleEmitter::SetEmitterData(const ParticleEmit& emitter) {
    if (name_.empty()) {
        return;
    }

    emitterSettings_.position  = emitter.translate;
    emitterSettings_.radius    = emitter.radius;
    emitterSettings_.count     = emitter.count;
    emitterSettings_.frequency = emitter.frequency;

    GPUParticleManager::GetInstance()->SetEmitterSphere(name_, emitter);
}