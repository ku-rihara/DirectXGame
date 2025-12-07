#include "GPUParticleSection.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "Function/GetFile.h"
#include "GPUParticle/GPUParticleManager.h"
#include "MathFunction.h"
#include "Primitive/IPrimitive.h"
#include <imgui.h>
#include <numbers>

void GPUParticleSection::Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex) {
    particleName_ = particleName;
    categoryName_ = categoryName;
    sectionIndex_ = sectionIndex;
    sectionName_  = particleName + std::to_string(sectionIndex);
    groupName_    = categoryName + "_" + sectionName_; // マネージャー用の一意な名前
    folderPath_   = baseFolderPath_ + categoryName_ + "/" + "Sections/" + particleName_ + "/";

    globalParameter_ = GlobalParameter::GetInstance();

    // パーティクルグループをマネージャーに作成
    GPUParticleManager::GetInstance()->CreatePrimitiveParticle(groupName_, PrimitiveType::Plane, 1024);

    ParameterInit();

    if (!globalParameter_->HasRegisters(sectionName_)) {
        globalParameter_->CreateGroup(sectionName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(sectionName_);
    } else {
        GetParams();
    }

    // Rail初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->Init(sectionName_ + "Emit");

    // デバッグライン
    debugLine_.reset(Line3D::Create(24));
    emitBoxTransform_.Init();

    // テクスチャ適用
    AdaptTexture();

    // ブレンドモード適用
    groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);

    // パラメータ適用
    ApplyParameters();

    currentTime_ = 0.0f;
    shouldEmit_  = false;
}

void GPUParticleSection::ParameterInit() {
    // Transformパラメータ初期化
    transformParams_.scaleMin     = {0.5f, 0.5f, 0.5f};
    transformParams_.scaleMax     = {1.5f, 1.5f, 1.5f};
    transformParams_.rotationMin  = {0.0f, 0.0f, 0.0f};
    transformParams_.rotationMax  = {360.0f, 360.0f, 360.0f};
    transformParams_.translateMin = {-1.0f, -1.0f, -1.0f};
    transformParams_.translateMax = {1.0f, 1.0f, 1.0f};

    // Physicsパラメータ初期化
    physicsParams_.velocityMin      = {-1.0f, -1.0f, -1.0f};
    physicsParams_.velocityMax      = {1.0f, 1.0f, 1.0f};
    physicsParams_.rotationSpeedMin = {0.0f, 0.0f, 0.0f};
    physicsParams_.rotationSpeedMax = {180.0f, 180.0f, 180.0f};
 
    // Appearanceパラメータ初期化
    appearanceParams_.colorMin         = {1.0f, 1.0f, 1.0f, 1.0f};
    appearanceParams_.colorMax         = {1.0f, 1.0f, 1.0f, 1.0f};
    appearanceParams_.lifeTimeMin      = 1.0f;
    appearanceParams_.lifeTimeMax      = 3.0f;

    // その他は既存通り
    emitterSettings_.position  = {0.0f, 0.0f, 0.0f};
    emitterSettings_.count     = 10;
    emitterSettings_.frequency = 1.0f;

    selectedTexturePath_ = "Resources/Texture/circle.png";
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
        return;
    }

    if (playState_ == PlayState::WAITING) {
        UpdateWaiting(actualDeltaTime);
        return;
    }

    RailMoveUpdate();
    UpdateEmitTransform();
    SetEmitLine();

    currentTime_ += actualDeltaTime;

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
        emitterData.emit          = shouldEmit_ ? 1 : 0;

        GPUParticleManager::GetInstance()->SetEmitterSphere(groupName_, emitterData);
    }

    ApplyParameters();
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
    if (isMoveForRail_) {
        emitBoxTransform_.translation_ = railManager_->GetWorldTransform().GetWorldPos();
    } else {
        emitBoxTransform_.translation_ = emitterSettings_.position;
    }

    emitBoxTransform_.scale_ = {
        transformParams_.translateMax.x - transformParams_.translateMin.x,
        transformParams_.translateMax.y - transformParams_.translateMin.y,
        transformParams_.translateMax.z - transformParams_.translateMin.z};

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
    if (!shouldEmit_ || !groupSettings_.isActive) {
        return;
    }

    GPUParticleManager::GetInstance()->Emit(groupName_);
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

    // リソースのバッファに直接書き込み
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

    // テクスチャ選択
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
            ImGui::DragFloat3("Min", &transformParams_.translateMin.x, 0.1f);
            ImGui::DragFloat3("Max", &transformParams_.translateMax.x, 0.1f);
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Physics Parameters")) {
        if (ImGui::TreeNode("Velocity")) {
            ImGui::DragFloat3("Min", &physicsParams_.velocityMin.x, 0.1f);
            ImGui::DragFloat3("Max", &physicsParams_.velocityMax.x, 0.1f);
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
            ImGui::DragFloat("Life Min", &appearanceParams_.lifeTimeMin, 0.1f, 0.1f, 10.0f);
            ImGui::DragFloat("Life Max", &appearanceParams_.lifeTimeMax, 0.1f, 0.1f, 10.0f);
            ImGui::TreePop();
        }
    }
}

void GPUParticleSection::EmitterSettingsEditor() {
    if (ImGui::CollapsingHeader("Emitter Settings")) {
        ImGui::DragFloat3("Position", &emitterSettings_.position.x, 0.1f);
        ImGui::DragInt("Count", reinterpret_cast<int*>(&emitterSettings_.count), 1, 1, 1000);
        ImGui::DragFloat("Frequency", &emitterSettings_.frequency, 0.01f, 0.01f, 10.0f);
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
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
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

    if (railManager_) {
        railManager_->SetRailMoveTime(0.0f);
    }
}

void GPUParticleSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;
}

bool GPUParticleSection::IsFinished() const {
    // パーティクルは終了条件がないため、常にfalseを返す
    // 必要に応じて寿命時間などを追加できる
    return false;
}