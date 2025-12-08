#include "ParticleSection.h"
#include "3d/ModelManager.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "Function/GetFile.h"
#include "ParticleManager.h"
#include <imgui.h>

void ParticleSection::Init(const std::string& particleName, const std::string& categoryName, int32_t sectionIndex) {
    particleName_ = particleName;
    categoryName_ = categoryName;
    sectionIndex_ = sectionIndex;
    sectionName_  = particleName + std::to_string(sectionIndex);
    groupName_    = categoryName + "_" + sectionName_;
    folderPath_   = baseFolderPath_ + categoryName_ + "/" + "Sections/" + particleName_ + "/";

    globalParameter_ = GlobalParameter::GetInstance();

    // ParticleParameterハンドラーを作成
    parameterHandler_ = std::make_unique<ParticleParameter>();

    // パラメータの登録と取得
    if (!globalParameter_->HasRegisters(sectionName_)) {
        parameterHandler_->particleName_ = sectionName_;
        parameterHandler_->Init();
        globalParameter_->CreateGroup(sectionName_);
        RegisterAdditionalParams();
        globalParameter_->SyncParamForGroup(sectionName_);
    } else {
        parameterHandler_->GetParams();
        GetAdditionalParams();
    }

    // パーティクルグループ作成
    if (parameterHandler_->useModel_) {
        CreateModelParticle(parameterHandler_->modelFilePath_, parameterHandler_->maxParticleNum_);
    } else {
        CreatePrimitiveParticle(
            static_cast<PrimitiveType>(parameterHandler_->primitiveTypeInt_),
            parameterHandler_->maxParticleNum_);
    }

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

    // デバッグライン
    debugLine_.reset(Line3D::Create(24));
    emitBoxTransform_.Init();

    // スケールイージング初期化
    scaleEasing_.SetAdaptValue(&parameterHandler_->parameters_.scaleEaseParm.currentScaleOffset);

    // 各種適用
    AdaptTexture();
    AdaptEaseSettings();
    AdaptRailSettings();

    // ブレンドモード適用
    parameterHandler_->groupParamaters_.blendMode    = static_cast<BlendMode>(parameterHandler_->blendMode_);
    parameterHandler_->groupParamaters_.billBordType = static_cast<BillboardType>(parameterHandler_->billBordType_);
}

void ParticleSection::CreateModelParticle(const std::string& modelFilePath, int32_t maxnum) {
    // 既存のグループがあれば削除
    ParticleManager::GetInstance()->particleGroups_.erase(groupName_);

    // モデルパーティクルグループを作成
    ParticleManager::GetInstance()->CreateParticleGroup(groupName_, modelFilePath, maxnum);
}

void ParticleSection::CreatePrimitiveParticle(PrimitiveType primitiveType, int32_t maxnum) {
    // 既存のグループがあれば削除
    ParticleManager::GetInstance()->particleGroups_.erase(groupName_);

    // プリミティブパーティクルグループを作成
    ParticleManager::GetInstance()->CreatePrimitiveParticle(groupName_, primitiveType, maxnum);
}

void ParticleSection::SetTextureHandle(uint32_t handle) {
    ParticleManager::GetInstance()->SetTextureHandle(groupName_, handle);
}

void ParticleSection::RegisterAdditionalParams() {
    globalParameter_->Regist(sectionName_, "Start Time", &startTime_);
    timeModeSelector_.RegisterParam(sectionName_, globalParameter_);

    // Rail関連
    globalParameter_->Regist(sectionName_, "useRail", &useRail_);
    globalParameter_->Regist(sectionName_, "railFileName", &railFileName_);

    // スケールイージング
    globalParameter_->Regist(sectionName_, "useScaleEasing", &useScaleEasing_);
}

void ParticleSection::GetAdditionalParams() {
    startTime_ = globalParameter_->GetValue<float>(sectionName_, "Start Time");
    timeModeSelector_.GetParam(sectionName_, globalParameter_);

    useRail_      = globalParameter_->GetValue<bool>(sectionName_, "useRail");
    railFileName_ = globalParameter_->GetValue<std::string>(sectionName_, "railFileName");

    useScaleEasing_ = globalParameter_->GetValue<bool>(sectionName_, "useScaleEasing");
}

void ParticleSection::Update(float speedRate) {
    if (!parameterHandler_->groupParamaters_.isShot && playState_ != PlayState::PLAYING) {
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

    if (playState_ == PlayState::WAITING) {
        UpdateWaiting(actualDeltaTime);
        return;
    }

    // Rail更新
    if (useRail_ && railPlayer_->IsPlaying()) {
        railPlayer_->Update(speedRate);
        parameterHandler_->parameters_.emitPos = railPlayer_->GetCurrentPosition();
    }

    // スケールイージング更新
    if (useScaleEasing_) {
        scaleEasing_.Update(actualDeltaTime);
    }

    UpdateEmitTransform();
    SetEmitLine();

    Emit();
}

void ParticleSection::UpdateWaiting(float deltaTime) {
    elapsedTime_ += deltaTime;
    if (elapsedTime_ >= startTime_) {
        StartPlay();
    }
}

void ParticleSection::StartPlay() {
    playState_ = PlayState::PLAYING;

    // Rail再生開始
    if (useRail_ && !railFileName_.empty()) {
        railPlayer_->Play(railFileName_);
    }

    // スケールイージングリセット
    if (useScaleEasing_) {
        scaleEasing_.Reset();
    }
}

void ParticleSection::UpdateEmitTransform() {
    emitBoxTransform_.translation_ = parameterHandler_->parameters_.emitPos;

    emitBoxTransform_.scale_ = {
        parameterHandler_->parameters_.positionDist.max.x - parameterHandler_->parameters_.positionDist.min.x,
        parameterHandler_->parameters_.positionDist.max.y - parameterHandler_->parameters_.positionDist.min.y,
        parameterHandler_->parameters_.positionDist.max.z - parameterHandler_->parameters_.positionDist.min.z};

    emitBoxTransform_.UpdateMatrix();
}

void ParticleSection::SetEmitLine() {
#ifdef _DEBUG
    if (useRail_) {
        // Rail使用時のデバッグ表示
    } else {
        debugLine_->SetCubeWireframe(
            emitBoxTransform_.GetWorldPos(),
            emitBoxTransform_.scale_,
            Vector4::kWHITE());
    }
#endif
}

void ParticleSection::Emit() {
    currentTime_ += Frame::DeltaTime();

    if (currentTime_ >= parameterHandler_->intervalTime_ || parameterHandler_->groupParamaters_.isShot) {
        // 最大数チェック
        auto& group = ParticleManager::GetInstance()->particleGroups_[groupName_];
        if (static_cast<int32_t>(group.particles.size()) + parameterHandler_->particleCount_ > parameterHandler_->maxParticleNum_) {
            currentTime_ = 0.0f;
            return;
        }

        // スケールイージング使用時は現在のスケールを適用
        if (useScaleEasing_) {
            Vector3 currentScale                           = scaleEasing_.GetValue();
            parameterHandler_->parameters_.scaleDistV3.min = currentScale;
            parameterHandler_->parameters_.scaleDistV3.max = currentScale;

            // スカラースケール使用時
            if (parameterHandler_->parameters_.isScalerScale) {
                float scaleValue                             = (currentScale.x + currentScale.y + currentScale.z) / 3.0f;
                parameterHandler_->parameters_.scaleDist.min = scaleValue;
                parameterHandler_->parameters_.scaleDist.max = scaleValue;
            }
        }

        ParticleManager::GetInstance()->Emit(
            groupName_, parameterHandler_->parameters_, parameterHandler_->groupParamaters_, parameterHandler_->particleCount_);
        currentTime_ = 0.0f;
    }
}

void ParticleSection::ChangePrimitive(const PrimitiveType& primitiveType) {
    CreatePrimitiveParticle(primitiveType, parameterHandler_->maxParticleNum_);
    AdaptTexture();
}

void ParticleSection::AdaptEaseSettings() {
    if (!useScaleEasing_) {
        return;
    }

    auto& easeParm = parameterHandler_->parameters_.scaleEaseParm;

    // イージングタイプを設定
    scaleEasing_.SetType(static_cast<EasingType>(easeParm.easeTypeInt));
    scaleEasing_.SetMaxTime(easeParm.maxTime);

    // 開始値と終了値
    if (parameterHandler_->parameters_.isScalerScale) {
        Vector3 startScale = Vector3::OneVector() * easeParm.startValueF;
        Vector3 endScale   = Vector3::OneVector() * easeParm.endValueF.max;
        scaleEasing_.SetStartValue(startScale);
        scaleEasing_.SetEndValue(endScale);
    } else {
        scaleEasing_.SetStartValue(easeParm.startValueV3);
        scaleEasing_.SetEndValue(easeParm.endValueV3.max);
    }
}

void ParticleSection::AdaptRailSettings() {
    if (!useRail_ || railFileName_.empty()) {
        return;
    }
}

void ParticleSection::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("Section: " + sectionName_).c_str());
    ImGui::PushID(sectionName_.c_str());

    ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 100.0f);
    timeModeSelector_.SelectTimeModeImGui("Time Mode");

    // Rail設定
    if (ImGui::CollapsingHeader("Rail Settings")) {
        ImGui::Checkbox("Use Rail", &useRail_);

        if (useRail_) {
            std::string railDirectory = globalParameter_->GetDirectoryPath() + "RailEditor/Dates";

            // FileSelectorを使用してRailファイルを選択
            railFileSelector_.SelectFile(
                "Rail File",
                railDirectory,
                railFileName_,
                "",
                false);

            if (ImGui::Button("Preview Rail") && !railFileName_.empty()) {
                railPlayer_->Play(railFileName_);
            }
        }
    }

    // スケールイージング設定
    if (ImGui::CollapsingHeader("Scale Easing")) {
        ImGui::Checkbox("Use Scale Easing", &useScaleEasing_);

        if (useScaleEasing_) {
            auto& easeParm = parameterHandler_->parameters_.scaleEaseParm;

            ImGui::DragFloat("Max Time", &easeParm.maxTime, 0.01f, 0.0f, 10.0f);

            // イージングタイプ選択
            ImGuiEasingTypeSelector("Easing Type", easeParm.easeTypeInt);

            if (parameterHandler_->parameters_.isScalerScale) {
                ImGui::DragFloat("Start Scale", &easeParm.startValueF, 0.01f);
                ImGui::DragFloat("End Scale Max", &easeParm.endValueF.max, 0.01f);
            } else {
                ImGui::DragFloat3("Start Scale", &easeParm.startValueV3.x, 0.01f);
                ImGui::DragFloat3("End Scale Max", &easeParm.endValueV3.max.x, 0.01f);
            }

            ImGui::DragFloat("Back Ratio", &easeParm.backRatio, 0.01f, 0.0f, 5.0f);

            if (ImGui::Button("Apply Easing Settings")) {
                AdaptEaseSettings();
            }
        }
    }

    // Primitive/Model設定
    if (ImGui::CollapsingHeader("Primitive/Model Type")) {
        bool useModel = parameterHandler_->useModel_;
        ImGui::Checkbox("Use Model", &useModel);
        parameterHandler_->useModel_ = useModel;

        if (useModel) {
            // モデルフォルダ選択
            modelFileSelector_.SelectFile(
                "Model Folder",
                modelBasePath_,
                parameterHandler_->modelFilePath_,
                "",
                false);

            // モデルパスの表示
            if (!parameterHandler_->modelFilePath_.empty()) {
                std::string fullPath = parameterHandler_->modelFilePath_ + "/" + parameterHandler_->modelFilePath_ + ".obj";
                ImGui::Text("Full Path: %s", fullPath.c_str());
            }

            if (ImGui::Button("Apply Model") && !parameterHandler_->modelFilePath_.empty()) {

                std::string fullPath = parameterHandler_->modelFilePath_ + "/" + parameterHandler_->modelFilePath_ + ".obj";
                CreateModelParticle(fullPath, parameterHandler_->maxParticleNum_);
                AdaptTexture();
            }
        } else {
            const char* primitiveItems[] = {"Plane", "Ring", "Cylinder"};
            int primitiveType            = parameterHandler_->primitiveTypeInt_;
            if (ImGui::Combo("Primitive Type", &primitiveType, primitiveItems, IM_ARRAYSIZE(primitiveItems))) {
                parameterHandler_->primitiveTypeInt_ = primitiveType;
                ChangePrimitive(static_cast<PrimitiveType>(primitiveType));
            }
        }
    }

    // 既存のパーティクルパラメータ編集
    parameterHandler_->EditorUpdate();

    globalParameter_->ParamSaveForImGui(sectionName_, folderPath_);
    globalParameter_->ParamLoadForImGui(sectionName_, folderPath_);

    ImGui::PopID();
#endif
}

void ParticleSection::ImGuiTextureSelection() {
    parameterHandler_->ImGuiTextureSelection();
}

void ParticleSection::ApplyTexture(const std::string& textureName) {
    parameterHandler_->ApplyTexture(textureName);
}

void ParticleSection::AdaptTexture() {
    parameterHandler_->AdaptTexture();
}

void ParticleSection::SetTexture(uint32_t textureHandle) {
    ParticleManager::GetInstance()->SetTextureHandle(groupName_, textureHandle);
}

void ParticleSection::LoadData() {
    globalParameter_->LoadFile(sectionName_, folderPath_);
    globalParameter_->SyncParamForGroup(sectionName_);
    parameterHandler_->GetParams();
    GetAdditionalParams();
    AdaptEaseSettings();
    AdaptRailSettings();
}

void ParticleSection::SaveData() {
    globalParameter_->SaveFile(sectionName_, folderPath_);
}

void ParticleSection::Reset() {
    currentTime_ = 0.0f;
    elapsedTime_ = 0.0f;
    playState_   = PlayState::STOPPED;

    if (railPlayer_) {
        railPlayer_->Stop();
    }

    if (useScaleEasing_) {
        scaleEasing_.Reset();
    }
}

void ParticleSection::StartWaiting() {
    playState_   = PlayState::WAITING;
    elapsedTime_ = 0.0f;
}

bool ParticleSection::IsFinished() const {

    return false;
}