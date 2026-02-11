#include "GPUParticleSection.h"

using namespace KetaEngine;
#include "3D/Primitive/IPrimitive.h"
#include "Base/TextureManager.h"
#include "Frame/Frame.h"
#include "Function/GetFile.h"
#include "Particle/GPUParticle/GPUParticleManager.h"
#include "MathFunction.h"
#include <imgui.h>
#include <numbers>

void GPUParticleSection::Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex) {
    particleName_ = particleName;
    categoryName_ = categoryName;
    sectionIndex_ = sectionIndex;
    sectionName_  = particleName + std::to_string(sectionIndex);
    groupName_    = categoryName + "_" + sectionName_;
    folderPath_   = baseFolderPath_ + categoryName_ + "/" + "Sections/" + particleName_ + "/";

    globalParameter_ = GlobalParameter::GetInstance();

    GPUParticleManager::GetInstance()->CreatePrimitiveParticle(groupName_, PrimitiveType::Plane, 1024);

    ParameterInit();

    if (!globalParameter_->HasRegisters(sectionName_)) {
        globalParameter_->CreateGroup(sectionName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(sectionName_);
    } else {
        GetParams();
    }

    railManager_ = std::make_unique<RailManager>();
    railManager_->Init(sectionName_ + "Emit");

    debugLine_.reset(Line3D::Create(24));
    emitBoxTransform_.Init();

    AdaptTexture();
    groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);
    ApplyParameters();

    currentTime_ = 0.0f;
    shouldEmit_  = false;
}

void GPUParticleSection::ParameterInit() {
    billboardMode_       = static_cast<BillboardMode>(billboardModeInt_);
    selectedTexturePath_ = "Resources/Texture/circle.dds";
    startTime_           = 0.0f;
}

void GPUParticleSection::RegisterParams() {
    // Transform Parameters
    globalParameter_->Regist(sectionName_, "Scale Min", &transformParams_.scaleMin);
    globalParameter_->Regist(sectionName_, "Scale Max", &transformParams_.scaleMax);
    globalParameter_->Regist(sectionName_, "Rotation Min", &transformParams_.rotationMin);
    globalParameter_->Regist(sectionName_, "Rotation Max", &transformParams_.rotationMax);
    globalParameter_->Regist(sectionName_, "Translate Min", &transformParams_.translateMin);
    globalParameter_->Regist(sectionName_, "Translate Max", &transformParams_.translateMax);

    // Physics Parameters
    globalParameter_->Regist(sectionName_, "Velocity Min", &physicsParams_.velocityMin);
    globalParameter_->Regist(sectionName_, "Velocity Max", &physicsParams_.velocityMax);
    globalParameter_->Regist(sectionName_, "Rotation Speed Min", &physicsParams_.rotationSpeedMin);
    globalParameter_->Regist(sectionName_, "Rotation Speed Max", &physicsParams_.rotationSpeedMax);

    // Appearance Parameters
    globalParameter_->Regist(sectionName_, "Color Min", &appearanceParams_.colorMin);
    globalParameter_->Regist(sectionName_, "Color Max", &appearanceParams_.colorMax);
    globalParameter_->Regist(sectionName_, "LifeTime Min", &appearanceParams_.lifeTimeMin);
    globalParameter_->Regist(sectionName_, "LifeTime Max", &appearanceParams_.lifeTimeMax);

    // Emitter Settings
    globalParameter_->Regist(sectionName_, "Position", &emitterSettings_.position);
    globalParameter_->Regist(sectionName_, "Count", reinterpret_cast<int32_t*>(&emitterSettings_.count));
    globalParameter_->Regist(sectionName_, "Frequency", &emitterSettings_.frequency);

    // Group Settings
    globalParameter_->Regist(sectionName_, "Blend Mode", &blendModeIndex_);
    globalParameter_->Regist(sectionName_, "Is Active", &groupSettings_.isActive);
    globalParameter_->Regist(sectionName_, "Texture Path", &selectedTexturePath_);

    // Rail
    globalParameter_->Regist(sectionName_, "Is Move For Rail", &isMoveForRail_);
    globalParameter_->Regist(sectionName_, "Is Rail Loop", &isRailLoop_);
    globalParameter_->Regist(sectionName_, "Move Speed", &moveSpeed_);

    // Time
    globalParameter_->Regist(sectionName_, "Start Time", &startTime_);
    timeModeSelector_.RegisterParam(sectionName_, globalParameter_);

    // Billboard & Direction
    globalParameter_->Regist(sectionName_, "Billboard Mode", &billboardModeInt_);
    globalParameter_->Regist(sectionName_, "Align To Velocity", &groupSettings_.alignToVelocity);

    // UV Parameters
    globalParameter_->Regist(sectionName_, "UV Position", &uvParams_.uvPosition);
    globalParameter_->Regist(sectionName_, "UV Rotate", &uvParams_.uvRotate);
    globalParameter_->Regist(sectionName_, "UV Scroll Speed", &uvParams_.uvScrollSpeed);
    globalParameter_->Regist(sectionName_, "UV Scale", &uvParams_.uvScale);
    globalParameter_->Regist(sectionName_, "UV Flip X", &uvParams_.isFlipX);
    globalParameter_->Regist(sectionName_, "UV Flip Y", &uvParams_.isFlipY);
    globalParameter_->Regist(sectionName_, "UV Pixel Step", &uvParams_.pixelStep);
    globalParameter_->Regist(sectionName_, "UV Loop", &uvParams_.isUVLoop);
}

void GPUParticleSection::GetParams() {
    // Transform
    transformParams_.scaleMin     = globalParameter_->GetValue<Vector3>(sectionName_, "Scale Min");
    transformParams_.scaleMax     = globalParameter_->GetValue<Vector3>(sectionName_, "Scale Max");
    transformParams_.rotationMin  = globalParameter_->GetValue<Vector3>(sectionName_, "Rotation Min");
    transformParams_.rotationMax  = globalParameter_->GetValue<Vector3>(sectionName_, "Rotation Max");
    transformParams_.translateMin = globalParameter_->GetValue<Vector3>(sectionName_, "Translate Min");
    transformParams_.translateMax = globalParameter_->GetValue<Vector3>(sectionName_, "Translate Max");

    // Physics
    physicsParams_.velocityMin      = globalParameter_->GetValue<Vector3>(sectionName_, "Velocity Min");
    physicsParams_.velocityMax      = globalParameter_->GetValue<Vector3>(sectionName_, "Velocity Max");
    physicsParams_.rotationSpeedMin = globalParameter_->GetValue<Vector3>(sectionName_, "Rotation Speed Min");
    physicsParams_.rotationSpeedMax = globalParameter_->GetValue<Vector3>(sectionName_, "Rotation Speed Max");

    // Appearance
    appearanceParams_.colorMin    = globalParameter_->GetValue<Vector4>(sectionName_, "Color Min");
    appearanceParams_.colorMax    = globalParameter_->GetValue<Vector4>(sectionName_, "Color Max");
    appearanceParams_.lifeTimeMin = globalParameter_->GetValue<float>(sectionName_, "LifeTime Min");
    appearanceParams_.lifeTimeMax = globalParameter_->GetValue<float>(sectionName_, "LifeTime Max");

    emitterSettings_.position  = globalParameter_->GetValue<Vector3>(sectionName_, "Position");
    emitterSettings_.count     = globalParameter_->GetValue<int32_t>(sectionName_, "Count");
    emitterSettings_.frequency = globalParameter_->GetValue<float>(sectionName_, "Frequency");

    blendModeIndex_         = globalParameter_->GetValue<int>(sectionName_, "Blend Mode");
    groupSettings_.isActive = globalParameter_->GetValue<bool>(sectionName_, "Is Active");
    selectedTexturePath_    = globalParameter_->GetValue<std::string>(sectionName_, "Texture Path");

    isMoveForRail_ = globalParameter_->GetValue<bool>(sectionName_, "Is Move For Rail");
    isRailLoop_    = globalParameter_->GetValue<bool>(sectionName_, "Is Rail Loop");
    moveSpeed_     = globalParameter_->GetValue<float>(sectionName_, "Move Speed");

    startTime_ = globalParameter_->GetValue<float>(sectionName_, "Start Time");
    timeModeSelector_.GetParam(sectionName_, globalParameter_);

    billboardModeInt_              = globalParameter_->GetValue<int>(sectionName_, "Billboard Mode");
    billboardMode_                 = static_cast<BillboardMode>(billboardModeInt_);
    groupSettings_.alignToVelocity = globalParameter_->GetValue<bool>(sectionName_, "Align To Velocity");

    uvParams_.uvPosition    = globalParameter_->GetValue<Vector2>(sectionName_, "UV Position");
    uvParams_.uvRotate      = globalParameter_->GetValue<float>(sectionName_, "UV Rotate");
    uvParams_.uvScrollSpeed = globalParameter_->GetValue<Vector2>(sectionName_, "UV Scroll Speed");
    uvParams_.uvScale       = globalParameter_->GetValue<Vector2>(sectionName_, "UV Scale");
    uvParams_.isFlipX       = globalParameter_->GetValue<bool>(sectionName_, "UV Flip X");
    uvParams_.isFlipY       = globalParameter_->GetValue<bool>(sectionName_, "UV Flip Y");
    uvParams_.pixelStep     = globalParameter_->GetValue<float>(sectionName_, "UV Pixel Step");
    uvParams_.isUVLoop      = globalParameter_->GetValue<bool>(sectionName_, "UV Loop");
}

void GPUParticleSection::Update(float speedRate) {
    if (!groupSettings_.isActive) {
        return;
    }

    float actualDeltaTime;
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    if (playState_ == PlayState::STOPPED) {
        // 停止中はエミットフラグを必ずfalseに
        shouldEmit_  = false;
        currentTime_ = 0.0f;

        auto group = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
        if (group && group->emitSphereData) {
            group->emitSphereData->emit          = 0;
            group->emitSphereData->frequencyTime = 0.0f;
        }
        return;
    }

    if (playState_ == PlayState::WAITING) {
        UpdateWaiting(actualDeltaTime);
        return;
    }

    if (playState_ == PlayState::PAUSED) {
        // 一時停止中もエミットを停止
        shouldEmit_ = false;
        auto group  = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
        if (group && group->emitSphereData) {
            group->emitSphereData->emit = 0;
        }
        return;
    }

    // PLAYING
    if (playState_ == PlayState::PLAYING) {
        RailMoveUpdate();
        UpdateEmitTransform();
        SetEmitLine();

        currentTime_ += actualDeltaTime;

        // 前回のエミットからの経過時間を確認
        if (currentTime_ >= emitterSettings_.frequency) {
            shouldEmit_  = true;
            currentTime_ = 0.0f;
        } else {
            shouldEmit_ = false;
        }

        auto group = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
        if (group && group->emitSphereData) {
            ParticleEmit emitterData = *group->emitSphereData;

            if (isMoveForRail_) {
                emitterData.translate = railManager_->GetWorldTransform().GetWorldPos();
            } else {
                emitterData.translate = emitterSettings_.position;
            }

            emitterData.count         = emitterSettings_.count;
            emitterData.frequency     = emitterSettings_.frequency;
            emitterData.frequencyTime = currentTime_;
            // shouldEmit_がtrueの時のみエミット
            emitterData.emit = shouldEmit_ ? 1 : 0;

            GPUParticleManager::GetInstance()->SetEmitterSphere(groupName_, emitterData);
        }

        ApplyParameters();
    }
}
void GPUParticleSection::UpdateWaiting(float deltaTime) {
    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= startTime_) {
        StartPlay();
    }
}

void GPUParticleSection::StartPlay() {
    playState_ = PlayState::PLAYING;
}

void GPUParticleSection::RailMoveUpdate() {
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

void GPUParticleSection::UpdateEmitTransform() {
    Vector3 basePosition;

    if (isMoveForRail_) {
        basePosition = railManager_->GetWorldTransform().GetWorldPos();
    } else {
        basePosition = emitterSettings_.position;
    }

    Vector3 rangeCenter = (transformParams_.translateMax + transformParams_.translateMin) * 0.5f;
    Vector3 rangeSize   = transformParams_.translateMax - transformParams_.translateMin;

    emitBoxTransform_.translation_ = basePosition + rangeCenter;
    emitBoxTransform_.scale_       = rangeSize;

    emitBoxTransform_.UpdateMatrix();
}

void GPUParticleSection::SetEmitLine() {
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

void GPUParticleSection::Emit() {
    // 再生中でない場合はエミットしない
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // アクティブでない場合もエミットしない
    if (!groupSettings_.isActive) {
        return;
    }

    // shouldEmit_がtrueの時のみエミット
    if (!shouldEmit_) {
        return;
    }

    GPUParticleManager::GetInstance()->Emit(groupName_);

    // エミット後、即座にフラグをリセット
    shouldEmit_ = false;
}
void GPUParticleSection::StartRailEmit() {
    isStartRailMove_ = true;
    railManager_->SetRailMoveTime(0.0f);
    railManager_->SetIsRoop(isRailLoop_);
}

void GPUParticleSection::ApplyParameters() {
    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
    if (!group || !group->resourceData) {
        return;
    }

    auto& buffers = group->resourceData->GetEmitParamBuffers();

    // Transform
    if (auto* data = buffers.transformBuffer.mappedData) {
        data->scaleMin     = transformParams_.scaleMin;
        data->scaleMax     = transformParams_.scaleMax;
        data->rotationMin  = ToRadian(transformParams_.rotationMin);
        data->rotationMax  = ToRadian(transformParams_.rotationMax);
        data->translateMin = transformParams_.translateMin;
        data->translateMax = transformParams_.translateMax;
    }

    // Physics
    if (auto* data = buffers.physicsBuffer.mappedData) {
        data->velocityMin      = physicsParams_.velocityMin;
        data->velocityMax      = physicsParams_.velocityMax;
        data->rotationSpeedMin = ToRadian(physicsParams_.rotationSpeedMin);
        data->rotationSpeedMax = ToRadian(physicsParams_.rotationSpeedMax);
    }

    // Appearance
    if (auto* data = buffers.appearanceBuffer.mappedData) {
        data->colorMin    = appearanceParams_.colorMin;
        data->colorMax    = appearanceParams_.colorMax;
        data->lifeTimeMin = appearanceParams_.lifeTimeMin;
        data->lifeTimeMax = appearanceParams_.lifeTimeMax;
    }

    auto& uvAnimeData         = *group->material.GetUVAnimeData();
    uvAnimeData.uvPosition    = uvParams_.uvPosition;
    uvAnimeData.uvRotate      = uvParams_.uvRotate;
    uvAnimeData.uvScrollSpeed = uvParams_.uvScrollSpeed;
    uvAnimeData.uvScale       = uvParams_.uvScale;
    uvAnimeData.isFlipX       = uvParams_.isFlipX ? 1 : 0;
    uvAnimeData.isFlipY       = uvParams_.isFlipY ? 1 : 0;
    uvAnimeData.pixelStep     = uvParams_.pixelStep;
    uvAnimeData.isUVLoop      = uvParams_.isUVLoop ? 1 : 0;

    auto& perView = group->resourceData->GetPerViewBuffer();
    if (auto* data = perView.mappedData) {
        data->alignToVelocity = groupSettings_.alignToVelocity ? 1 : 0;
        data->billboardMode   = static_cast<int32_t>(billboardMode_);
    }
}

void GPUParticleSection::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Section: " + sectionName_).c_str());
    ImGui::PushID(sectionName_.c_str());

    ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 100.0f);
    timeModeSelector_.SelectTimeModeImGui("Time Mode");

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

    if (ImGui::CollapsingHeader("Billboard & Direction")) {
        const char* modes[] = {"None", "Billboard", "Y-Axis"};
        if (ImGui::Combo("Billboard Mode", &billboardModeInt_, modes, IM_ARRAYSIZE(modes))) {
            billboardMode_ = static_cast<BillboardMode>(billboardModeInt_);
        }
        ImGui::Checkbox("Align To Velocity", &groupSettings_.alignToVelocity);
    }

    if (ImGui::CollapsingHeader("UV Animation")) {
        ImGui::DragFloat2("Position", &uvParams_.uvPosition.x, 0.01f);
        ImGui::SliderAngle("Rotate", &uvParams_.uvRotate);
        ImGui::DragFloat2("Scroll Speed", &uvParams_.uvScrollSpeed.x, 0.01f);
        ImGui::DragFloat2("Scale", &uvParams_.uvScale.x, 0.01f, 0.1f, 10.0f);
        ImGui::Checkbox("Flip X", &uvParams_.isFlipX);
        ImGui::SameLine();
        ImGui::Checkbox("Flip Y", &uvParams_.isFlipY);
        ImGui::DragFloat("Pixel Step", &uvParams_.pixelStep, 0.001f, 0.0f, 0.1f);
        ImGui::Checkbox("Loop", &uvParams_.isUVLoop);
    }

    ImGuiTextureSelection();

    ImGui::PopID();
#endif
}

void GPUParticleSection::EmitParameterEditor() {
    if (ImGui::CollapsingHeader("Transform Parameters")) {
        if (ImGui::TreeNode("Scale")) {
            ImGui::DragFloat3("Min", &transformParams_.scaleMin.x, 0.01f);
            ImGui::DragFloat3("Max", &transformParams_.scaleMax.x, 0.01f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation (Degree)")) {
            ImGui::DragFloat3("Min", &transformParams_.rotationMin.x, 1.0f, -360.0f, 360.0f);
            ImGui::DragFloat3("Max", &transformParams_.rotationMax.x, 1.0f, -360.0f, 360.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Position Offset")) {
            ImGui::DragFloat3("Min", &transformParams_.translateMin.x, 0.01f);
            ImGui::DragFloat3("Max", &transformParams_.translateMax.x, 0.01f);
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Physics Parameters")) {
        if (ImGui::TreeNode("Velocity")) {
            ImGui::DragFloat3("Min", &physicsParams_.velocityMin.x, 0.01f);
            ImGui::DragFloat3("Max", &physicsParams_.velocityMax.x, 0.01f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation Speed (Degree/sec)")) {
            ImGui::DragFloat3("Min", &physicsParams_.rotationSpeedMin.x, 1.0f, 0.0f, 720.0f);
            ImGui::DragFloat3("Max", &physicsParams_.rotationSpeedMax.x, 1.0f, 0.0f, 720.0f);
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Appearance Parameters")) {
        if (ImGui::TreeNode("Color")) {
            ImGui::ColorEdit4("Min", &appearanceParams_.colorMin.x);
            ImGui::ColorEdit4("Max", &appearanceParams_.colorMax.x);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("LifeTime & Fade")) {
            ImGui::DragFloat("Life Min", &appearanceParams_.lifeTimeMin, 0.01f, 0.1f, 10.0f);
            ImGui::DragFloat("Life Max", &appearanceParams_.lifeTimeMax, 0.01f, 0.1f, 10.0f);
            ImGui::TreePop();
        }
    }
}

void GPUParticleSection::EmitterSettingsEditor() {
    if (ImGui::CollapsingHeader("Emitter Settings")) {
        ImGui::DragFloat3("Position", &emitterSettings_.position.x, 0.01f);
        ImGui::DragInt("Count", reinterpret_cast<int*>(&emitterSettings_.count), 1, 1, 1000);
        ImGui::DragFloat("Frequency", &emitterSettings_.frequency, 0.001f, 0.0f, 10.0f);
    }
}

void GPUParticleSection::ImGuiTextureSelection() {
    static int selectedIndex           = 0;
    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    DisplayFileSelection("Select Texture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);
        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}

void GPUParticleSection::DisplayFileSelection(
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

void GPUParticleSection::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".dds";
    uint32_t handle      = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(groupName_, handle);
}

void GPUParticleSection::AdaptTexture() {
    if (selectedTexturePath_.empty()) {
        return;
    }
    uint32_t handle = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(groupName_, handle);
}

void GPUParticleSection::SetTexture(uint32_t textureHandle) {
    GPUParticleManager::GetInstance()->SetTextureHandle(groupName_, textureHandle);
}

void GPUParticleSection::SetEmitterData(const ParticleEmit& emitter) {
    emitterSettings_.position  = emitter.translate;
    emitterSettings_.count     = emitter.count;
    emitterSettings_.frequency = emitter.frequency;

    GPUParticleManager::GetInstance()->SetEmitterSphere(groupName_, emitter);
}

void GPUParticleSection::LoadData() {
    globalParameter_->LoadFile(sectionName_, folderPath_);
    globalParameter_->SyncParamForGroup(sectionName_);
    GetParams();
}

void GPUParticleSection::SaveData() {
    globalParameter_->SaveFile(sectionName_, folderPath_);
}

void GPUParticleSection::Reset() {
    currentTime_ = 0.0f;
    elapsedTime_ = 0.0f;
    shouldEmit_  = false;
    playState_   = PlayState::STOPPED;

    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
    if (group && group->emitSphereData) {
        group->emitSphereData->emit          = 0;
        group->emitSphereData->frequencyTime = 0.0f;
    }

    if (railManager_) {
        railManager_->SetRailMoveTime(0.0f);
    }
}

void GPUParticleSection::Pause() {
    if (playState_ == PlayState::PLAYING || playState_ == PlayState::WAITING) {
        playState_ = PlayState::PAUSED;

        shouldEmit_ = false;
        auto group  = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
        if (group && group->emitSphereData) {
            group->emitSphereData->emit          = 0;
            group->emitSphereData->frequencyTime = 0.0f;
        }
    }
}

void GPUParticleSection::Resume() {
    if (playState_ == PlayState::PAUSED) {
        playState_ = PlayState::PLAYING;

        currentTime_ = 0.0f;
        shouldEmit_  = false;
    }
}

void GPUParticleSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;
    shouldEmit_  = false;

    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(groupName_);
    if (group && group->emitSphereData) {
        group->emitSphereData->emit = 0;
    }
}
bool GPUParticleSection::IsFinished() const {
    return false;
}