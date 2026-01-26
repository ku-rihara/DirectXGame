#include "PlayerComboAttackPreview.h"
// Frame
#include "Frame/Frame.h"
// Behavior
#include "Player/ComboAttackBehavior/ComboAttackAction.h"
// ComboCreator
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
// Player
#include "Player/Player.h"
// Utility
#include "utility/TimeLine/TimelineDrawer.h"
// std
#include <unordered_set>

PlayerComboAttackPreview::PlayerComboAttackPreview(
    Player* player,
    PlayerComboAttackData* startAttackData,
    PreviewMode mode,
    KetaEngine::TimelineDrawer* timeline)
    : BaseComboAttackBehavior("ComboAttackPreview", player), timeline_(timeline), currentAttackData_(startAttackData), previewMode_(mode) {

    attackController_ = player->GetComboAttackController();
    Init();
}

void PlayerComboAttackPreview::Init() {
    // 初期位置を保存
    initialPlayerPosition_ = pOwner_->GetWorldPosition();
    initialPlayerRotation_ = pOwner_->GetBaseTransform().rotation_;

    currentTime_            = 0.0f;
    buttonInputTimer_       = 0.0f;
    isWaitingForNextAttack_ = false;
    hasSimulatedInput_      = false;
    isFinished_             = false;
    shouldStop_             = false;

    // 最初の攻撃を実行
    if (currentAttackData_) {
        pOwner_->ChangeComboBehavior(
            std::make_unique<ComboAttackAction>(pOwner_, currentAttackData_));

        // 次の攻撃データを取得
        std::string nextAttackName = currentAttackData_->GetAttackParam().nextAttackType;
        if (!nextAttackName.empty() && nextAttackName != "None") {
            nextAttackData_ = attackController_->GetAttackByName(nextAttackName);
        }
    }

    if (timeline_) {
        timeline_->SetPlaying(true);
        timeline_->SetCurrentFrame(0);
    }
}

void PlayerComboAttackPreview::Update(float atkSpeed) {
    if (shouldStop_) {
        // 停止リクエストがあった場合
        if (timeline_) {
            timeline_->SetPlaying(false);
            timeline_->SetCurrentFrame(0);
        }

        pOwner_->SetWorldPosition(initialPlayerPosition_);
        pOwner_->SetRotation(initialPlayerRotation_);
        pOwner_->ChangeCombBoRoot();

        isFinished_ = true;
        return;
    }

    currentTime_ += atkSpeed;

    switch (previewMode_) {
    case PreviewMode::SINGLE:
        UpdateSinglePreview(atkSpeed);
        break;

    case PreviewMode::CHAIN:
        UpdateChainPreview(atkSpeed);
        break;
    }
}

void PlayerComboAttackPreview::UpdateSinglePreview(float atkSpeed) {
    if (!currentAttackData_ || !timeline_) {
        return;
    }

    // タイムラインが終了したらループ
    if (timeline_->GetCurrentFrame() >= timeline_->GetEndFrame()) {
        // 攻撃を再実行
        pOwner_->ChangeComboBehavior(
            std::make_unique<ComboAttackAction>(pOwner_, currentAttackData_));

        timeline_->SetCurrentFrame(0);
        currentTime_ = 0.0f;
    }
}

void PlayerComboAttackPreview::UpdateChainPreview(float atkSpeed) {
    if (!currentAttackData_ || !timeline_) {
        return;
    }

    buttonInputTimer_ += atkSpeed;

    // ボタン入力シミュレーション
    if (!isWaitingForNextAttack_ && !hasSimulatedInput_) {
        SimulateButtonInput();
    }

    // 攻撃が終了したら次の攻撃へ
    if (timeline_->GetCurrentFrame() >= timeline_->GetEndFrame()) {
        // 次の攻撃がある場合
        if (nextAttackData_ && nextAttackData_->GetGroupName() != "None" && !nextAttackData_->GetGroupName().empty()) {
            AdvanceToNextAttack();
        } else {
            // チェーンの最後に到達したら最初に戻る
            auto* firstAttack = FindFirstAttackInChain();
            if (firstAttack) {
                currentAttackData_ = firstAttack;

                pOwner_->ChangeComboBehavior(
                    std::make_unique<ComboAttackAction>(pOwner_, currentAttackData_));

                timeline_->SetCurrentFrame(0);
                currentTime_       = 0.0f;
                buttonInputTimer_  = 0.0f;
                hasSimulatedInput_ = false;

                // 次の攻撃データを取得
                std::string nextAttackName = currentAttackData_->GetAttackParam().nextAttackType;
                if (!nextAttackName.empty() && nextAttackName != "None") {
                    nextAttackData_ = attackController_->GetAttackByName(nextAttackName);
                } else {
                    nextAttackData_ = nullptr;
                }
            }
        }
    }
}

void PlayerComboAttackPreview::SimulateButtonInput() {
    if (!currentAttackData_ || !nextAttackData_) {
        return;
    }

    int32_t currentFrame = static_cast<int32_t>(currentTime_ / KetaEngine::Frame::DeltaTime());

    const auto& attackParam   = currentAttackData_->GetAttackParam();
    int32_t precedeInputFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.precedeInputTime);

    if (currentFrame >= precedeInputFrame && currentFrame % buttonInputInterval_ == 0) {
        hasSimulatedInput_      = true;
        isWaitingForNextAttack_ = true;
    }
}

void PlayerComboAttackPreview::AdvanceToNextAttack() {
    if (!nextAttackData_) {
        return;
    }

    currentAttackData_ = nextAttackData_;

    pOwner_->ChangeComboBehavior(
        std::make_unique<ComboAttackAction>(pOwner_, currentAttackData_));

    if (timeline_) {
        timeline_->SetCurrentFrame(0);
    }

    currentTime_            = 0.0f;
    buttonInputTimer_       = 0.0f;
    hasSimulatedInput_      = false;
    isWaitingForNextAttack_ = false;

    // 次の攻撃データを取得
    std::string nextAttackName = currentAttackData_->GetAttackParam().nextAttackType;
    if (!nextAttackName.empty() && nextAttackName != "None") {
        nextAttackData_ = attackController_->GetAttackByName(nextAttackName);
    } else {
        nextAttackData_ = nullptr;
    }
}

PlayerComboAttackData* PlayerComboAttackPreview::FindFirstAttackInChain() {
    if (!attackController_) {
        return nullptr;
    }

    std::unordered_set<std::string> visited;
    std::string currentName = currentAttackData_ ? currentAttackData_->GetGroupName() : "";

    if (currentName.empty()) {
        return nullptr;
    }

    const auto& allAttacks = attackController_->GetAllAttacks();

    for (const auto& attack : allAttacks) {
        if (!attack)
            continue;

        PlayerComboAttackData* current = attack.get();
        visited.clear();

        while (current && visited.find(current->GetGroupName()) == visited.end()) {
            if (current->GetGroupName() == currentName) {
                return attack.get();
            }

            visited.insert(current->GetGroupName());

            std::string nextName = current->GetAttackParam().nextAttackType;
            if (nextName.empty() || nextName == "None") {
                break;
            }

            current = attackController_->GetAttackByName(nextName);
        }
    }

    return currentAttackData_;
}

void PlayerComboAttackPreview::Debug() {
#ifdef _DEBUG

#endif
}