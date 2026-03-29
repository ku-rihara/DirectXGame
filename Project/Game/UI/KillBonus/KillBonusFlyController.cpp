#include "KillBonusFlyController.h"

#include "3d/ViewProjection.h"
#include "math/Matrix4x4.h"

#include <algorithm>
#include <imgui.h>

void KillBonusFlyController::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void KillBonusFlyController::Update(float deltaTime) {
    for (auto& entry : entries_) {
        entry->Update(deltaTime);
    }

    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [](const std::unique_ptr<KillBonusFlyEntry>& e) { return e->IsFinished(); }),
        entries_.end());
}

void KillBonusFlyController::SpawnFromPending(
    const Vector3& worldPos, const KetaEngine::ViewProjection& vp) {
    const Vector2 startPos = ScreenTransform(worldPos, vp);

    KillBonusFlyParam param;
    param.endPos            = endPos_;
    param.controlOffset       = controlOffset_;
    param.controlXOffsetRange = controlXOffsetRange_;
    param.controlOffset2      = controlOffset2_;
    param.controlXOffsetRange2 = controlXOffsetRange2_;
    param.duration          = duration_;
    param.spriteScale       = spriteScale_;
    param.spriteTexture     = "UI/KillRecovery.dds";
    param.worldStartPos     = worldPos;

    const float bonusValue = pendingComboBonusValue_;

    auto entry = std::make_unique<KillBonusFlyEntry>();
    entry->Init(startPos, param);
    entry->SetOnReachCallback([this, bonusValue]() {
        if (onReachCallback_) {
            onReachCallback_(bonusValue);
        }
        if (onGaugeAnimCallback_) {
            onGaugeAnimCallback_();
        }
    });

    entries_.push_back(std::move(entry));
}

void KillBonusFlyController::RegisterParams() {
    globalParameter_->Regist(groupName_, "endPos", &endPos_);
    globalParameter_->Regist(groupName_, "controlOffset",        &controlOffset_);
    globalParameter_->Regist(groupName_, "controlXOffsetRange",  &controlXOffsetRange_);
    globalParameter_->Regist(groupName_, "controlOffset2",       &controlOffset2_);
    globalParameter_->Regist(groupName_, "controlXOffsetRange2", &controlXOffsetRange2_);
    globalParameter_->Regist(groupName_, "duration", &duration_);
    globalParameter_->Regist(groupName_, "spriteScale", &spriteScale_);
}

void KillBonusFlyController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("終点スクリーン座標", &endPos_.x, 1.0f);
        ImGui::DragFloat2("第1制御点オフセット（始点側）", &controlOffset_.x, 1.0f);
        ImGui::DragFloat("第1制御点 X 範囲",              &controlXOffsetRange_, 1.0f);
        ImGui::DragFloat2("第2制御点オフセット（終点側）", &controlOffset2_.x, 1.0f);
        ImGui::DragFloat("第2制御点 X 範囲",              &controlXOffsetRange2_, 1.0f);
        ImGui::DragFloat("移動時間(秒)", &duration_, 0.01f);
        ImGui::DragFloat2("スプライトスケール", &spriteScale_.x, 0.01f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
