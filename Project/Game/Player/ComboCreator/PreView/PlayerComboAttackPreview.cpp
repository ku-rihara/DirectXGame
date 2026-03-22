#include "PlayerComboAttackPreview.h"
// attackData
#include "../PlayerComboAttackData.h"
// Frame
#include "Frame/Frame.h"
// Behavior
#include "Player/Behavior/ComboAttackBehavior/ComboAttackAction.h"
#include "Player/Behavior/ComboAttackBehavior/ComboAttackRoot.h"
// AttackController
#include "Player/ComboCreator/PlayerComboAttackController.h"
// Player
#include "Player/Player.h"
// utility
#include "utility/TimeLine/TimelineDrawer.h"
// EffectEditorSuite
#include "Editor/EffectEditorSuite/EffectEditorSuite.h"
// imGui
#include <imgui.h>

void PlayerComboAttackPreview::Init(
    Player* player,
    PlayerComboAttackController* controller,
    KetaEngine::TimelineDrawer* timeline) {

    player_           = player;
    attackController_ = controller;
    timeline_         = timeline;

    currentMode_ = AttackPreviewMode::NONE;

    ResetState();
}

void PlayerComboAttackPreview::Update(float deltaTime) {
    if (currentMode_ == AttackPreviewMode::NONE || !player_ || !attackController_) {
        return;
    }

    attackElapsedTime_ += deltaTime;

    if (currentMode_ == AttackPreviewMode::SINGLE) {
        UpdateSingleMode();
    }

    // EffectEditor経由での演出再生
    UpdatePreviewRenditions();
}

void PlayerComboAttackPreview::UpdateSingleMode() {
    if (!currentAttackData_) {
        return;
    }

    KetaEngine::TimelineDrawer* tl = currentAttackData_->GetTimeline();
    if (!tl) {
        return;
    }

    // タイムラインが終了したらループ
    if (tl->GetCurrentFrame() >= tl->GetEndFrame()) {
        ExecuteAttack(currentAttackData_);
        attackElapsedTime_ = 0.0f;
    }
}

void PlayerComboAttackPreview::ExecuteAttack(PlayerComboAttackData* attackData) {
    if (!attackData || !player_) {
        return;
    }

    currentAttackData_ = attackData;

    // ビヘイビア初期化前に位置を設定してワールド行列を即時更新する
    // （SetMoveEasing内のGetWorldPosition()がmatWorld_から読むため、
    //   UpdateMatrix()を呼ばないとtranslation_の変更が反映されない）
    if (startFromOrigin_) {
        player_->SetWorldPosition(previewInitPos_);
        player_->UpdateWorldMatrix();
    }

    // プレイヤーのコンボビヘイビアを攻撃アクションに変更
    // Init()内のSetMoveEasing()がGetWorldPosition()でpreviewInitPos_を正しくキャプチャする
    player_->ChangeComboBehavior(
        std::make_unique<ComboAttackAction>(player_, attackData));

    // タイマーリセット
    attackElapsedTime_ = 0.0f;

    // 演出再生フラグリセット
    ResetRenditionFlags();

    // タイムラインをリセットして再生
    KetaEngine::TimelineDrawer* tl = attackData->GetTimeline();
    if (tl) {
        tl->SetCurrentFrame(0);
        tl->SetPlaying(true);
    }
}

void PlayerComboAttackPreview::StartPreview(PlayerComboAttackData* attackData, AttackPreviewMode mode) {
    if (!attackData || !player_ || mode == AttackPreviewMode::NONE) {
        return;
    }

    // 初期状態を保存
    SaveInitialState();

    currentMode_ = mode;
    ExecuteAttack(attackData);
}

void PlayerComboAttackPreview::StopPreview() {
    // タイムライン停止
    KetaEngine::TimelineDrawer* tl = currentAttackData_ ? currentAttackData_->GetTimeline() : nullptr;
    if (tl) {
        tl->SetPlaying(false);
        tl->SetCurrentFrame(0);
    }

    // プレイヤーを初期状態に戻す
    RestoreInitialState();

    // プレイヤーをコンボルートに戻す
    if (player_) {
        player_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(player_));
    }

    // 状態リセット
    currentMode_ = AttackPreviewMode::NONE;
    ResetState();
}

void PlayerComboAttackPreview::ResetState() {
    currentAttackData_ = nullptr;
    attackElapsedTime_ = 0.0f;
}

void PlayerComboAttackPreview::SaveInitialState() {
    if (!player_) {
        return;
    }

    initialPosition_ = player_->GetWorldPosition();
    initialRotation_ = player_->GetBaseTransform().rotation_;
}

void PlayerComboAttackPreview::RestoreInitialState() {
    if (!player_) {
        return;
    }

    player_->SetWorldPosition(initialPosition_);
    player_->SetRotation(initialRotation_);
}

void PlayerComboAttackPreview::DrawUI() {
    if (!ImGui::CollapsingHeader("プレビュー設定")) {
        return;
    }

    ImGui::PushID("PreviewSettings");

    // プレビュー状態表示
    if (currentMode_ == AttackPreviewMode::SINGLE) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "モード: 単体再生中");
    } else {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "モード: 停止中");
    }

    if (currentAttackData_) {
        ImGui::Text("現在の攻撃: %s", currentAttackData_->GetGroupName().c_str());
        ImGui::Separator();
        ImGui::Text("経過時間: %.2f秒", attackElapsedTime_);
    }

    ImGui::Separator();

    // 原点スタート設定
    ImGui::Checkbox("原点(InitPos)からスタート", &startFromOrigin_);
    if (startFromOrigin_) {
        ImGui::DragFloat3("InitPos", &previewInitPos_.x, 0.1f);
    }

    ImGui::Separator();

    // プレビュー停止ボタン
    if (IsPlaying()) {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button("プレビュー停止", ImVec2(-1, 0))) {
            StopPreview();
        }

        ImGui::PopStyleColor(3);
    }

    ImGui::PopID();
}

std::string PlayerComboAttackPreview::GetCurrentAttackName() const {
    if (currentAttackData_) {
        return currentAttackData_->GetGroupName();
    }
    return "";
}

void PlayerComboAttackPreview::ResetRenditionFlags() {
    renditionPlayed_.fill(false);
    objAnimPlayed_.fill(false);
    prevFrame_ = 0;
}

void PlayerComboAttackPreview::UpdatePreviewRenditions() {
    if (!pEditorSuite_ || !currentAttackData_) {
        return;
    }

    KetaEngine::TimelineDrawer* timeline = currentAttackData_->GetTimeline();
    if (!timeline) {
        return;
    }

    int32_t currentFrame = timeline->GetCurrentFrame();

    // フレームが逆行した場合（ループ等）は再生フラグをリセット
    if (currentFrame < prevFrame_) {
        ResetRenditionFlags();
    }

    const auto& renditionData = currentAttackData_->GetRenditionData();

    // 通常演出 (RenditionType → EffectEditorType + category)
    struct RendMapping {
        PlayerAttackRenditionData::Type rendType;
        KetaEngine::EffectEditorType    editorType;
        const char*                     category;
    };
    static const RendMapping kMapping[] = {
        {PlayerAttackRenditionData::Type::CameraAction,      KetaEngine::EffectEditorType::Camera,     "Common"},
        {PlayerAttackRenditionData::Type::HitStop,           KetaEngine::EffectEditorType::TimeScale,  "Common"},
        {PlayerAttackRenditionData::Type::ShakeAction,       KetaEngine::EffectEditorType::Shake,      "Common"},
        {PlayerAttackRenditionData::Type::ParticleEffect,    KetaEngine::EffectEditorType::Particle,   "Player"},
        {PlayerAttackRenditionData::Type::RibbonTrailEffect, KetaEngine::EffectEditorType::RibbonTrail,"Player"},
    };

    for (const auto& m : kMapping) {
        size_t idx = static_cast<size_t>(m.rendType);
        if (renditionPlayed_[idx]) {
            continue;
        }
        const auto& param = renditionData.GetRenditionParamFromType(m.rendType);
        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }
        int32_t effectFrame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        if (prevFrame_ < effectFrame && currentFrame >= effectFrame) {
            pEditorSuite_->PlayEffect(m.editorType, param.fileName, m.category);
            renditionPlayed_[idx] = true;
        }
    }

    // オブジェクトアニメーション
    struct ObjAnimMapping {
        PlayerAttackRenditionData::ObjAnimationType animType;
        const char*                                 category;
    };
    static const ObjAnimMapping kObjMapping[] = {
        {PlayerAttackRenditionData::ObjAnimationType::Head,      "Player"},
        {PlayerAttackRenditionData::ObjAnimationType::RightHand, "RightHand"},
        {PlayerAttackRenditionData::ObjAnimationType::LeftHand,  "LeftHand"},
        {PlayerAttackRenditionData::ObjAnimationType::MainHead,  "MainHead"},
    };

    for (const auto& m : kObjMapping) {
        size_t idx = static_cast<size_t>(m.animType);
        if (objAnimPlayed_[idx]) {
            continue;
        }
        const auto& param = renditionData.GetObjAnimationParamFromType(m.animType);
        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }
        int32_t effectFrame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        if (prevFrame_ < effectFrame && currentFrame >= effectFrame) {
            pEditorSuite_->PlayEffect(KetaEngine::EffectEditorType::ObjEaseAnimation, param.fileName, m.category);
            objAnimPlayed_[idx] = true;
        }
    }

    prevFrame_ = currentFrame;
}
