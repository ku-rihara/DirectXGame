#include "base/TextureManager.h"
#include "Function/GetFile.h"
#include "GPUParticleManager.h"
#include "GPUParticleParameter.h"
#include <imgui.h>

void GPUParticleParameter::Init(const std::string& name) {
    particleName_ = name;

    ParameterInit();

    // GlobalParameter初期化
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(folderPath_ + "/" + particleName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(folderPath_ + "/" + particleName_);

    // Rail初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->Init(particleName_ + "Emit");

    // テクスチャ適用
    AdaptTexture();

    // ブレンドモード適用
    groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);
}

void GPUParticleParameter::ParameterInit() {
    // デフォルト値設定
    emitParams_.scaleMin = {0.5f, 0.5f, 0.5f};
    emitParams_.scaleMax = {1.5f, 1.5f, 1.5f};

    emitParams_.rotationMin = {0.0f, 0.0f, 0.0f};
    emitParams_.rotationMax = {360.0f, 360.0f, 360.0f};

    emitParams_.rotationSpeedMin = {0.0f, 0.0f, 0.0f};
    emitParams_.rotationSpeedMax = {180.0f, 180.0f, 180.0f};

    emitParams_.translateMin = {-1.0f, -1.0f, -1.0f};
    emitParams_.translateMax = {1.0f, 1.0f, 1.0f};

    emitParams_.velocityMin = {-1.0f, -1.0f, -1.0f};
    emitParams_.velocityMax = {1.0f, 1.0f, 1.0f};

    emitParams_.colorMin = {1.0f, 1.0f, 1.0f, 1.0f};
    emitParams_.colorMax = {1.0f, 1.0f, 1.0f, 1.0f};

    emitParams_.lifeTimeMin = 1.0f;
    emitParams_.lifeTimeMax = 3.0f;

    emitterSettings_.position  = {0.0f, 0.0f, 0.0f};
    emitterSettings_.radius    = 1.0f;
    emitterSettings_.count     = 10;
    emitterSettings_.frequency = 1.0f;

    selectedTexturePath_ = "resources/Texture/circle.png";
}

void GPUParticleParameter::RegisterParams() {
    std::string groupName = folderPath_ + "/" + particleName_;

    // Emit Parameters
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

    // Emitter Settings
    globalParameter_->Regist(groupName, "Position", &emitterSettings_.position);
    globalParameter_->Regist(groupName, "Radius", &emitterSettings_.radius);
    globalParameter_->Regist(groupName, "Count", reinterpret_cast<int32_t*>(&emitterSettings_.count));
    globalParameter_->Regist(groupName, "Frequency", &emitterSettings_.frequency);

    // Group Settings
    globalParameter_->Regist(groupName, "Blend Mode", &blendModeIndex_);
    globalParameter_->Regist(groupName, "Is Active", &groupSettings_.isActive);

    // Texture
    globalParameter_->Regist(groupName, "Texture Path", &selectedTexturePath_);

    // Rail
    globalParameter_->Regist(groupName, "Is Move For Rail", &isMoveForRail_);
    globalParameter_->Regist(groupName, "Is Rail Loop", &isRailLoop_);
    globalParameter_->Regist(groupName, "Move Speed", &moveSpeed_);
}

void GPUParticleParameter::EditorUpdate() {
#ifdef _DEBUG
    ImGui::Begin(("GPU Particle: " + particleName_).c_str());
    ImGui::PushID(particleName_.c_str());

    // Emit Parameters
    EmitParameterEditor();

    // Emitter Settings
    EmitterSettingsEditor();

    // Group Settings
    if (ImGui::CollapsingHeader("Group Settings")) {
        ImGui::Checkbox("Is Active", &groupSettings_.isActive);

        const char* blendModeItems[] = {"None", "Add", "Multiply", "Subtractive", "Screen"};
        if (ImGui::Combo("Blend Mode", &blendModeIndex_, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
            groupSettings_.blendMode = static_cast<BlendMode>(blendModeIndex_);
        }
    }

    // Rail Settings
    if (ImGui::CollapsingHeader("Rail Movement")) {
        ImGui::Checkbox("Is Move For Rail", &isMoveForRail_);
        ImGui::Checkbox("Is Rail Loop", &isRailLoop_);
        ImGui::DragFloat("Move Speed", &moveSpeed_, 0.1f);

        ImGui::SeparatorText("Control Points:");
        railManager_->ImGuiEdit();
    }

    // Texture Selection
    ImGuiTextureSelection();

    // Preset Change
    ParticlePresetChange();

    // パラメータ適用ボタン
    if (ImGui::Button("Apply Parameters")) {
        ApplyParameters();
    }

    // Save/Load
    std::string groupName = folderPath_ + "/" + particleName_;
    globalParameter_->ParamSaveForImGui(groupName, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName, folderPath_);

    ImGui::PopID();
    ImGui::End();
#endif
}

void GPUParticleParameter::EmitParameterEditor() {
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

void GPUParticleParameter::EmitterSettingsEditor() {
    if (ImGui::CollapsingHeader("Emitter Settings")) {
        ImGui::DragFloat3("Position", &emitterSettings_.position.x, 0.1f);
        ImGui::DragFloat("Radius", &emitterSettings_.radius, 0.1f, 0.0f, 100.0f);
        ImGui::DragInt("Count", reinterpret_cast<int*>(&emitterSettings_.count), 1, 1, 1000);
        ImGui::DragFloat("Frequency", &emitterSettings_.frequency, 0.01f, 0.01f, 10.0f);
    }
}

void GPUParticleParameter::ImGuiTextureSelection() {
    static int selectedIndex           = 0;
    std::vector<std::string> filenames = GetFileNamesForDirectory(textureFilePath_);

    DisplayFileSelection("Select Texture", filenames, selectedIndex, [this](const std::string& selectedFile) {
        ApplyTexture(selectedFile);
        ImGui::Text("Texture Applied: %s", selectedFile.c_str());
    });
}

void GPUParticleParameter::DisplayFileSelection(
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

void GPUParticleParameter::ParticlePresetChange() {
    static int selectedIndex = 0;

    // プリセットファイルのリストを取得
    std::vector<std::string> filenames; // GetFileNamesForDirectory(folderPath_) などで取得

    DisplayFileSelection("Select Preset", filenames, selectedIndex, [this](const std::string& selectedFile) {
        std::string groupName = folderPath_ + "/" + particleName_;
        globalParameter_->CopyGroup(folderPath_ + "/" + selectedFile, groupName);
        globalParameter_->SyncParamForGroup(groupName);
        ApplyParameters();
        ImGui::Text("Preset Loaded: %s", selectedFile.c_str());
    });
}

void GPUParticleParameter::ApplyParameters() {
    // EmitParameterをシェーダーに送る形式に変換
    EmitParameter shaderParams;

    // Degree to Radian conversion
    shaderParams.rotationMin = {
        emitParams_.rotationMin.x * 3.14159f / 180.0f,
        emitParams_.rotationMin.y * 3.14159f / 180.0f,
        emitParams_.rotationMin.z * 3.14159f / 180.0f};
    shaderParams.rotationMax = {
        emitParams_.rotationMax.x * 3.14159f / 180.0f,
        emitParams_.rotationMax.y * 3.14159f / 180.0f,
        emitParams_.rotationMax.z * 3.14159f / 180.0f};

    shaderParams.rotationSpeedMin = {
        emitParams_.rotationSpeedMin.x * 3.14159f / 180.0f,
        emitParams_.rotationSpeedMin.y * 3.14159f / 180.0f,
        emitParams_.rotationSpeedMin.z * 3.14159f / 180.0f};
    shaderParams.rotationSpeedMax = {
        emitParams_.rotationSpeedMax.x * 3.14159f / 180.0f,
        emitParams_.rotationSpeedMax.y * 3.14159f / 180.0f,
        emitParams_.rotationSpeedMax.z * 3.14159f / 180.0f};

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

    // GPUParticleManagerに適用
    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(particleName_);
    if (group && group->resourceData) {
        group->resourceData->UpdateEmitParamData(shaderParams);
    }

    // Emitter設定の適用
    ParticleEmit emitterData;
    emitterData.translate     = emitterSettings_.position;
    emitterData.radius        = emitterSettings_.radius;
    emitterData.count         = emitterSettings_.count;
    emitterData.frequency     = emitterSettings_.frequency;
    emitterData.frequencyTime = 0.0f;
    emitterData.emit          = 0;

    GPUParticleManager::GetInstance()->SetEmitterSphere(particleName_, emitterData);
}

void GPUParticleParameter::ApplyTexture(const std::string& textureName) {
    selectedTexturePath_ = textureFilePath_ + "/" + textureName + ".png";
    uint32_t handle      = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(particleName_, handle);
}

void GPUParticleParameter::AdaptTexture() {
    if (selectedTexturePath_.empty()) {
        return;
    }
    uint32_t handle = TextureManager::GetInstance()->LoadTexture(selectedTexturePath_);
    GPUParticleManager::GetInstance()->SetTextureHandle(particleName_, handle);
}