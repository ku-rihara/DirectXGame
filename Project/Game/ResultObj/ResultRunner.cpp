#include "ResultRunner.h"
#include "Frame/Frame.h"
#include <imgui.h>

void ResultRunner::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 初期Z位置セット
    for (int32_t i = 0; i < kRunnerCount; ++i) {
        currentZ_[i] = config_.loopStartZ + static_cast<float>(i) * config_.zSpacing;
    }

    // Object3DAnimation 生成
    runnerAnimations_.clear();
    runnerAnimations_.resize(kRunnerCount);
    for (int32_t i = 0; i < kRunnerCount; ++i) {
        runnerAnimations_[i].reset(
            KetaEngine::Object3DAnimation::CreateModel("Enemy/NormalEnemyRun.gltf"));
        runnerAnimations_[i]->Init();
        runnerAnimations_[i]->SetLoop(true);
        runnerAnimations_[i]->transform_.translation_ = {config_.basePos.x, config_.basePos.y, currentZ_[i]};
        runnerAnimations_[i]->transform_.rotation_    = {0.f, config_.rotateY, 0.f};
        runnerAnimations_[i]->transform_.scale_       = config_.scale;
    }
}

void ResultRunner::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    for (int32_t i = 0; i < kRunnerCount; ++i) {
        // Z方向に前進（増加方向）
        currentZ_[i] += config_.runSpeed * dt;
        if (currentZ_[i] > config_.loopEndZ) {
            currentZ_[i] = config_.loopStartZ;
        }

        runnerAnimations_[i]->transform_.translation_.x = config_.basePos.x;
        runnerAnimations_[i]->transform_.translation_.y = config_.basePos.y;
        runnerAnimations_[i]->transform_.translation_.z = currentZ_[i];
        runnerAnimations_[i]->transform_.rotation_.y    = config_.rotateY;
        runnerAnimations_[i]->transform_.scale_         = config_.scale;
    }
}

void ResultRunner::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePos",    &config_.basePos);
    globalParameter_->Regist(groupName_, "zSpacing",   &config_.zSpacing);
    globalParameter_->Regist(groupName_, "runSpeed",   &config_.runSpeed);
    globalParameter_->Regist(groupName_, "loopStartZ", &config_.loopStartZ);
    globalParameter_->Regist(groupName_, "loopEndZ",   &config_.loopEndZ);
    globalParameter_->Regist(groupName_, "rotateY",    &config_.rotateY);
    globalParameter_->Regist(groupName_, "scale",      &config_.scale);
}

void ResultRunner::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat3("BasePos",    &config_.basePos.x,   0.1f);
        ImGui::DragFloat ("ZSpacing",   &config_.zSpacing,    0.1f);
        ImGui::DragFloat ("RunSpeed",   &config_.runSpeed,    0.1f);
        ImGui::DragFloat ("LoopStartZ", &config_.loopStartZ,  0.1f);
        ImGui::DragFloat ("LoopEndZ",   &config_.loopEndZ,    0.1f);
        ImGui::DragFloat ("RotateY",    &config_.rotateY,     0.01f);
        ImGui::DragFloat3("Scale",      &config_.scale.x,     0.01f);

        ImGui::Separator();
        ImGui::Text("--- CurrentPos ---");
        for (int32_t i = 0; i < kRunnerCount; ++i) {
            ImGui::Text("Runner[%d]  X:%.2f  Y:%.2f  Z:%.2f",
                i,
                runnerAnimations_[i]->transform_.translation_.x,
                runnerAnimations_[i]->transform_.translation_.y,
                runnerAnimations_[i]->transform_.translation_.z);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
