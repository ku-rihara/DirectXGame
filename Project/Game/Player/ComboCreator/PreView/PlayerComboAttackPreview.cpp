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
// std
#include <unordered_set>

void PlayerComboAttackPreview::Init(
    Player* player,
    PlayerComboAttackController* controller,
    KetaEngine::TimelineDrawer* timeline) {

    player_           = player;
    attackController_ = controller;
    timeline_         = timeline;

    currentMode_         = AttackPreviewMode::NONE;
    buttonInputInterval_ = 5;

    ResetState();
}

void PlayerComboAttackPreview::Update(float deltaTime) {
    if (currentMode_ == AttackPreviewMode::NONE || !player_ || !attackController_) {
        return;
    }

    attackElapsedTime_ += deltaTime;
    buttonInputTimer_ += deltaTime;

    switch (currentMode_) {
    case AttackPreviewMode::SINGLE:
        UpdateSingleMode();
        break;

    case AttackPreviewMode::CONTINUOUS:
        UpdateContinuousMode();
        break;

    default:
        break;
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

void PlayerComboAttackPreview::UpdateContinuousMode() {
    if (!currentAttackData_) {
        return;
    }

    // 次の攻撃への移行判定
    CheckAndAdvanceToNextAttack();

    // タイムラインが終了したかチェック
    KetaEngine::TimelineDrawer* tl = currentAttackData_->GetTimeline();
    if (tl && tl->GetCurrentFrame() >= tl->GetEndFrame()) {

        // 次の攻撃がある場合
        if (!nextAttackName_.empty() && nextAttackName_ != "None") {
            PlayerComboAttackData* nextAttack = attackController_->GetAttackByName(nextAttackName_);
            if (nextAttack) {
                ExecuteAttack(nextAttack);
                return;
            }
        }

        // チェーンの最後に到達したら最初に戻る
        // 現在の攻撃から攻撃チェーンの最初を探す
        PlayerComboAttackData* firstAttack = FindFirstAttackInChain(currentAttackData_);
        if (firstAttack) {
            ExecuteAttack(firstAttack);
        } else {
            StopPreview();
        }
    }
}

bool PlayerComboAttackPreview::ShouldTriggerNextAttack() {
    if (!currentAttackData_ || nextAttackName_.empty() || nextAttackName_ == "None") {
        return false;
    }

    // 既に入力シミュレーション済みなら何もしない
    if (hasSimulatedInput_) {
        return false;
    }

    const auto& attackParam = currentAttackData_->GetAttackParam();
    attackParam;
    //// 先行入力時間に達しているかチェック
    //if (attackElapsedTime_ < attackParam.timingParam.precedeInputTime) {
    //    return false;
    //}

    // ボタン連打間隔をチェック
    int32_t currentFrame = static_cast<int32_t>(buttonInputTimer_ / KetaEngine::Frame::DeltaTime());

    if (currentFrame >= buttonInputInterval_) {
        hasSimulatedInput_   = true;
        isWaitingTransition_ = true;
        return true;
    }

    return false;
}

void PlayerComboAttackPreview::CheckAndAdvanceToNextAttack() {
    if (!currentAttackData_) {
        return;
    }

    // 入力シミュレーション判定
    if (ShouldTriggerNextAttack()) {
        // フラグを立てるだけ（実際の遷移はタイムライン終了時）
        isWaitingTransition_ = true;
    }
}

void PlayerComboAttackPreview::ExecuteAttack(PlayerComboAttackData* attackData) {
    if (!attackData || !player_) {
        return;
    }

    // 原点スタートが有効なら毎回InitPosに戻す
    if (startFromOrigin_) {
        player_->SetWorldPosition(previewInitPos_);
    }

    currentAttackData_ = attackData;

    // 次の攻撃名を取得
    nextAttackName_ = ""; /* = current->GetAttackParam().nextAttackType;*/

    // プレイヤーのコンボビヘイビアを攻撃アクションに変更
    player_->ChangeComboBehavior(
        std::make_unique<ComboAttackAction>(player_, attackData));

    // タイマーリセット
    attackElapsedTime_   = 0.0f;
    buttonInputTimer_    = 0.0f;
    hasSimulatedInput_   = false;
    isWaitingTransition_ = false;

    // 演出再生フラグリセット
    ResetRenditionFlags();

    // タイムラインをリセットして再生
    KetaEngine::TimelineDrawer* tl = attackData->GetTimeline();
    if (tl) {
        tl->SetCurrentFrame(0);
        tl->SetPlaying(true);
    }
}

PlayerComboAttackData* PlayerComboAttackPreview::FindFirstAttackInChain(PlayerComboAttackData* currentAttack) {
    if (!attackController_ || !currentAttack) {
        return nullptr;
    }

    const auto& allAttacks = attackController_->GetAllAttacks();
    std::unordered_set<std::string> visited;
    std::string targetName = currentAttack->GetGroupName();

    // 全攻撃をチェックして、チェーンの最初を探す
    for (const auto& attack : allAttacks) {
        if (!attack)
            continue;

        visited.clear();
        PlayerComboAttackData* current = attack.get();

        // このattackから辿って現在の攻撃に到達できるかチェック
        while (current && visited.find(current->GetGroupName()) == visited.end()) {
            if (current->GetGroupName() == targetName) {
                // このチェーンに属しているattackが最初
                return attack.get();
            }

            visited.insert(current->GetGroupName());

            std::string nextName = ""; /* = current->GetAttackParam().nextAttackType;*/
            if (nextName.empty() || nextName == "None") {
                break;
            }

            current = attackController_->GetAttackByName(nextName);
        }
    }

    // 見つからない場合は現在の攻撃を返す
    return currentAttack;
}

void PlayerComboAttackPreview::StartPreview(PlayerComboAttackData* attackData, AttackPreviewMode mode) {
    if (!attackData || !player_ || mode == AttackPreviewMode::NONE) {
        return;
    }

    // 初期状態を保存
    SaveInitialState();

    // 原点スタートが有効なら初期位置に移動
    if (startFromOrigin_) {
        player_->SetWorldPosition(previewInitPos_);
    }

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
    nextAttackName_.clear();
    attackElapsedTime_   = 0.0f;
    buttonInputTimer_    = 0.0f;
    hasSimulatedInput_   = false;
    isWaitingTransition_ = false;
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
    const char* modeText = "停止中";
    ImVec4 modeColor     = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

    switch (currentMode_) {
    case AttackPreviewMode::SINGLE:
        modeText  = "単体再生中";
        modeColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        break;
    case AttackPreviewMode::CONTINUOUS:
        modeText  = "連続再生中";
        modeColor = ImVec4(0.2f, 0.5f, 1.0f, 1.0f);
        break;
    default:
        break;
    }

    ImGui::TextColored(modeColor, "モード: %s", modeText);

    if (currentAttackData_) {
        ImGui::Text("現在の攻撃: %s", currentAttackData_->GetGroupName().c_str());

        if (!nextAttackName_.empty() && nextAttackName_ != "None") {
            ImGui::Text("次の攻撃: %s", nextAttackName_.c_str());
        } else {
            ImGui::TextDisabled("次の攻撃: なし");
        }

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

    // ボタン連打間隔の設定（連続モード用）
    ImGui::Text("連続再生設定:");
    ImGui::DragInt("ボタン連打間隔(フレーム)", &buttonInputInterval_, 1.0f, 1, 60);

    ImGui::Separator();

    // プレビュー停止ボタン
    if (IsPlaying()) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button("プレビュー停止", ImVec2(-1, 0))) {
            StopPreview();
        }

        ImGui::PopStyleColor(3);

        // デバッグ情報
        if (ImGui::TreeNode("デバッグ情報")) {
            ImGui::Text("入力シミュレーション済み: %s", hasSimulatedInput_ ? "はい" : "いいえ");
            ImGui::Text("遷移待ち: %s", isWaitingTransition_ ? "はい" : "いいえ");
            ImGui::Text("ボタンタイマー: %.2f秒", buttonInputTimer_);
            ImGui::TreePop();
        }
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