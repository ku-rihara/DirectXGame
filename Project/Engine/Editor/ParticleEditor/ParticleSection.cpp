#include "ParticleSection.h"
#include "3d/ModelManager.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "ParticleManager.h"
#include <imgui.h>

using namespace KetaEngine;

void ParticleSection::Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex) {
    particleName_ = particleName;
    sectionIndex_ = sectionIndex;
    groupName_    = particleName + std::to_string(sectionIndex);
    folderPath_   = baseFolderPath_ + categoryName + "/" + "Sections/" + particleName_ + "/";

    sectionParam_ = std::make_unique<ParticleSectionParameter>();

    globalParameter_ = GlobalParameter::GetInstance();

    // グループ作成
    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        sectionParam_->RegisterParams(globalParameter_, groupName_);
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        sectionParam_->AdaptParameters(globalParameter_, groupName_);
    }

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

    // デバッグライン
    debugLine_.reset(Line3D::Create(24));
    emitBoxTransform_.Init();

    // パーティクルグループが存在しない場合は作成
    if (ParticleManager::GetInstance()->particleGroups_.find(groupName_) == ParticleManager::GetInstance()->particleGroups_.end()) {

        // デフォルトでPlaneプリミティブを作成
        CreatePrimitiveParticle(PrimitiveType::Plane, sectionParam_->GetMaxParticleNum());
    }

}

//*----------------------------- Playback Control -----------------------------*//

void ParticleSection::Play() {
    playState_   = PlayState::PLAYING;
    elapsedTime_ = 0.0f;
    currentTime_ = 0.0f;

    auto& railFileName = sectionParam_->GetRailFileName();
    if (sectionParam_->GetIsRailMove() && !railFileName.empty()) {
        railPlayer_->Play(railFileName);
    }
}

void ParticleSection::Pause() {
    if (playState_ == PlayState::PLAYING) {
        playState_ = PlayState::STOPPED;
    }
}

void ParticleSection::Stop() {
    playState_   = PlayState::STOPPED;
    currentTime_ = 0.0f;
    elapsedTime_ = 0.0f;

    if (railPlayer_) {
        railPlayer_->Stop();
    }
}

void ParticleSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;
    currentTime_ = 0.0f;
}

//*----------------------------- Update -----------------------------*//

void ParticleSection::Update(float speedRate) {
    if (!sectionParam_->GetGroupParameters().isShot && playState_ != PlayState::PLAYING && playState_ != PlayState::WAITING) {
        return;
    }

    float actualDeltaTime;
    switch (static_cast<TimeMode>(sectionParam_->GetTimeModeSelector().GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    if (playState_ == PlayState::WAITING) {
        UpdateWaiting(actualDeltaTime);
        return;
    }

    if (playState_ == PlayState::PLAYING) {
        if (sectionParam_->GetIsRailMove() && railPlayer_->IsPlaying()) {
            railPlayer_->Update(speedRate);
            sectionParam_->SetEmitPos(railPlayer_->GetCurrentPosition());
        }

        UpdateEmitTransform();
        SetEmitLine();
        EmitInternal();
    }
}

void ParticleSection::UpdateWaiting(float deltaTime) {
    elapsedTime_ += deltaTime;
    if (elapsedTime_ >= sectionParam_->GetTimingParam().startTime) {
        StartPlay();
    }
}

void ParticleSection::StartPlay() {
    playState_ = PlayState::PLAYING;

    auto& railFileName = sectionParam_->GetRailFileName();
    if (sectionParam_->GetIsRailMove() && !railFileName.empty()) {
        railPlayer_->Play(railFileName);
    }
}

void ParticleSection::UpdateEmitTransform() {
    emitBoxTransform_.translation_ = sectionParam_->GetParticleParameters().emitPos;

    // scale=max-min
    emitBoxTransform_.scale_ = 
        sectionParam_->GetParticleParameters().positionDist.max - 
        sectionParam_->GetParticleParameters().positionDist.min,
       
    emitBoxTransform_.UpdateMatrix();
}

void ParticleSection::SetEmitLine() {
#ifdef _DEBUG
    if (sectionParam_->GetIsRailMove()) {
        // Rail使用時のデバッグ表示
    } else {
        debugLine_->SetCubeWireframe(
            emitBoxTransform_.GetWorldPos(),
            emitBoxTransform_.scale_,
            Vector4::kWHITE());
    }
#endif
}

//*----------------------------- Emit Methods -----------------------------*//

void ParticleSection::EmitInternal() {
    // パーティクルグループが存在するか確認
    auto& groups = ParticleManager::GetInstance()->particleGroups_;
    if (groups.find(groupName_) == groups.end()) {
        return;
    }

    currentTime_ += Frame::DeltaTime();

    if (currentTime_ >= sectionParam_->GetIntervalTime() || sectionParam_->GetGroupParameters().isShot) {
        auto& group = groups[groupName_];

        // パーティクル数制限チェック
        if (static_cast<int32_t>(group.particles.size()) + sectionParam_->GetParticleCount() > sectionParam_->GetMaxParticleNum()) {
            currentTime_ = 0.0f;
            return;
        }


        ParticleManager::GetInstance()->Emit(
            groupName_,
            sectionParam_->GetParticleParameters(),
            sectionParam_->GetGroupParameters(),
            sectionParam_->GetParticleCount());

        currentTime_ = 0.0f;
    }
}

//*----------------------------- Primitive/Model Management -----------------------------*//

void ParticleSection::ChangePrimitive(const PrimitiveType& primitiveType) {
    CreatePrimitiveParticle(primitiveType, sectionParam_->GetMaxParticleNum());
    ApplyTextureToManager();
}


//*----------------------------- Editor UI -----------------------------*//

void ParticleSection::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Section: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    // Playback状態表示
    ImGui::Separator();
    ImGui::Text("Playback State:");
    switch (playState_) {
    case PlayState::STOPPED:
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "STOPPED");
        break;
    case PlayState::WAITING:
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "WAITING (%.2f / %.2f)",
            elapsedTime_, sectionParam_->GetTimingParam().startTime);
        break;
    case PlayState::PLAYING:
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "PLAYING");
        break;
    }

    // Primitive/Model設定
    if (ImGui::CollapsingHeader("Primitive/Model Type")) {
        bool useModel = sectionParam_->IsUseModel();
        ImGui::Checkbox("Use Model", &useModel);

        if (useModel) {
            modelFileSelector_.SelectFile(
                "Model Folder",
                modelBasePath_,
                const_cast<std::string&>(sectionParam_->GetModelFilePath()),
                "",
                false);

            if (!sectionParam_->GetModelFilePath().empty()) {
                std::string fullPath = sectionParam_->GetModelFilePath() + "/" + sectionParam_->GetModelFilePath() + ".obj";
                ImGui::Text("Full Path: %s", fullPath.c_str());
            }

            if (ImGui::Button("Apply Model") && !sectionParam_->GetModelFilePath().empty()) {
                std::string fullPath = sectionParam_->GetModelFilePath() + "/" + sectionParam_->GetModelFilePath() + ".obj";
                CreateModelParticle(fullPath, sectionParam_->GetMaxParticleNum());
                ApplyTextureToManager();
            }
        } else {
            const char* primitiveItems[] = {"Plane", "Ring", "Cylinder"};
            int primitiveType            = sectionParam_->GetPrimitiveTypeInt();

            ImGui::Combo("Primitive Type", &primitiveType, primitiveItems, IM_ARRAYSIZE(primitiveItems));

            ImGui::Text("Note: Primitive changes apply on Init or manual Apply");

            if (ImGui::Button("Apply Primitive")) {
                ChangePrimitive(static_cast<PrimitiveType>(primitiveType));
            }
        }
    }

    // 全パラメータ編集
    sectionParam_->AdjustParam();

    // セーブ・ロード
    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

    ImGui::PopID();
#endif
}

void ParticleSection::InitAdaptTexture() {
    sectionParam_->InitAdaptTexture();
    ApplyTextureToManager();
}

void ParticleSection::ApplyTextureToManager() {
    if (!sectionParam_->GetSelectedTexturePath().empty()) {
        uint32_t textureHandle = TextureManager::GetInstance()->LoadTexture(sectionParam_->GetSelectedTexturePath());
        ParticleManager::GetInstance()->SetTextureHandle(groupName_, textureHandle);
    }
}

void ParticleSection::SetTexture(uint32_t textureHandle) {
    ParticleManager::GetInstance()->SetTextureHandle(groupName_, textureHandle);
}

//*----------------------------- File Operations -----------------------------*//

void ParticleSection::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    sectionParam_->AdaptParameters(globalParameter_, groupName_);
    ApplyTextureToManager();
}

void ParticleSection::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

//*----------------------------- State Management -----------------------------*//

void ParticleSection::Reset() {
    Stop();
}

bool ParticleSection::IsFinished() const {
    return false;
}

void ParticleSection::CreateModelParticle(const std::string& modelFilePath, int32_t maxnum) {
    ParticleManager::GetInstance()->particleGroups_.erase(groupName_);
    ParticleManager::GetInstance()->CreateParticleGroup(groupName_, modelFilePath, maxnum);
}

void ParticleSection::CreatePrimitiveParticle(PrimitiveType primitiveType, int32_t maxnum) {
    ParticleManager::GetInstance()->particleGroups_.erase(groupName_);
    ParticleManager::GetInstance()->CreatePrimitiveParticle(groupName_, primitiveType, maxnum);
}

void ParticleSection::SetTextureHandle(uint32_t handle) {
    ParticleManager::GetInstance()->SetTextureHandle(groupName_, handle);
}