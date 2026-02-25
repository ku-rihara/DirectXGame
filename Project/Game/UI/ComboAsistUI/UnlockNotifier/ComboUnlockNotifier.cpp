#include "ComboUnlockNotifier.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
#include <algorithm>
#include <imgui.h>

///==========================================================
/// 初期化
///==========================================================
void ComboUnlockNotifier::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    InitBackSprite();
}

///==========================================================
/// 毎フレーム更新
///==========================================================
void ComboUnlockNotifier::Update(float deltaTime) {
    for (auto& card : cards_) {
        if (!card) {
            continue;
        }

        UpdateCardState(*card, deltaTime);
        ApplyScaleYToCard(*card);

        // DONE になったカードを削除マーク
        if (card->state == NotifyCard::State::DONE) {
            card.reset();
        }
    }

    // 消滅済みカードを除去
    cards_.erase(
        std::remove_if(cards_.begin(), cards_.end(),
            [](const std::unique_ptr<NotifyCard>& c) { return !c; }),
        cards_.end());

    RearrangeCards();

    // UIエレメントの更新
    for (auto& card : cards_) {
        if (!card) {
            continue;
        }
        for (auto& btn : card->buttonUIs) {
            if (btn)
                btn->Update();
        }
        for (auto& arrow : card->arrowUIs) {
            if (arrow)
                arrow->Update();
        }
    }
}

///==========================================================
/// カード状態更新
///==========================================================
void ComboUnlockNotifier::UpdateCardState(NotifyCard& card, float deltaTime) {
    switch (card.state) {
    case NotifyCard::State::OPENING:
        scaleYEasing_.Update(deltaTime);
        if (scaleYEasing_.IsFinished()) {
            card.state = NotifyCard::State::DISPLAYING;

            if (card.player) {
                auto& queue = card.player->GetAutoComboQueue();
                queue.Clear();
                for (auto* data : card.pendingAttacks) {
                    queue.Enqueue(data);
                }
            }

            if (card.totalAttackCount == 0) {
                StartCloseAnimation(card);
            }
        }
        break;
    case NotifyCard::State::DISPLAYING:
        break;
    case NotifyCard::State::CLOSING:
        scaleYEasing_.Update(deltaTime);
        if (scaleYEasing_.IsFinished()) {
            card.state = NotifyCard::State::DONE;
        }
        break;
    case NotifyCard::State::DONE:
        break;
    }
}

///==========================================================
/// スケールYをUIに適用
///==========================================================
void ComboUnlockNotifier::ApplyScaleYToCard(NotifyCard& card) {
    if (backgroundSprite) {
        backgroundSprite->transform_.scale.y = 1.0f * scaleY_;
        // scaleXは常に1
        backgroundSprite->transform_.scale.x = 1.0f;
    }
    if (notifierTextSprite) {
        notifierTextSprite->transform_.scale.y = 1.0f * scaleY_;
        // scaleXは常に1
        notifierTextSprite->transform_.scale.x = 1.0f;
    }
    if (card.conditionIconSprite) {
        card.conditionIconSprite->transform_.scale.y = card.conditionIconBaseScaleY * scaleY_;
    }
    for (auto& btn : card.buttonUIs) {
        if (btn)
            btn->SetScaleY(scaleY_);
    }
    for (auto& arrow : card.arrowUIs) {
        if (arrow)
            arrow->SetScaleY(scaleY_);
    }
}

///==========================================================
/// クローズアニメーション開始（1→0）
///==========================================================
void ComboUnlockNotifier::StartCloseAnimation(NotifyCard& card) {
    scaleYEasing_.SetIsStartEndReverse(true);
    scaleYEasing_.SetAdaptValue(&scaleY_);
    scaleYEasing_.Reset();
    card.state = NotifyCard::State::CLOSING;
}

///==========================================================
/// 攻撃解放通知
///==========================================================
void ComboUnlockNotifier::OnAttackUnlocked(
    const std::string& unlockedAttackName,
    const LayoutParam& layoutParam,
    PlayerComboAttackController* attackController,
    Player* player) {

    if (!attackController) {
        return;
    }

    PlayerComboAttackData* unlockedData = attackController->GetAttackByName(unlockedAttackName);
    if (!unlockedData) {
        return;
    }

    auto condition = unlockedData->GetAttackParam().triggerParam.condition;
    auto steps     = BuildLinearPath(unlockedAttackName, condition, attackController);

    if (steps.empty()) {
        return;
    }

    auto card    = std::make_unique<NotifyCard>();
    card->player = player;

    // 自動実行用攻撃データを収集
    if (player) {
        for (const auto& step : steps) {
            PlayerComboAttackData* data = attackController->GetAttackByName(step.attackName);
            if (data) {
                card->pendingAttacks.push_back(data);
            }
        }
        card->totalAttackCount = static_cast<int32_t>(card->pendingAttacks.size());
    }

    // スケールYイージング設定（Open: 0→1）
    scaleYEasing_.Init(kScaleYEasingFile_);
    scaleYEasing_.SetAdaptValue(&scaleY_);
    scaleYEasing_.Reset();

    int32_t cardIndex = static_cast<int32_t>(cards_.size());
    PopulateCard(*card, steps, layoutParam, cardIndex, condition);
    cards_.push_back(std::move(card));
}

///==========================================================
/// 攻撃自動実行時に通知UIにリアクションさせる
///==========================================================
void ComboUnlockNotifier::NotifyAttackExecuted(const std::string& attackName) {
    for (auto& card : cards_) {
        if (!card || card->state != NotifyCard::State::DISPLAYING) {
            continue;
        }

        // ボタンアニメーション再生
        for (auto& btn : card->buttonUIs) {
            if (btn)
                btn->TryPlayPushScaling(attackName);
        }

        // このカードの pendingAttacks に含まれる攻撃か確認
        bool belongsToCard = false;
        for (auto* data : card->pendingAttacks) {
            if (data && data->GetGroupName() == attackName) {
                belongsToCard = true;
                break;
            }
        }

        if (!belongsToCard) {
            continue;
        }

        card->executedAttackCount++;
        if (card->executedAttackCount >= card->totalAttackCount) {
            StartCloseAnimation(*card);
        }
    }
}

///==========================================================
/// 始点まで遡って直線パスを構築
///==========================================================
std::vector<ComboPathBuilder::ComboStep> ComboUnlockNotifier::BuildLinearPath(
    const std::string& unlockedAttackName,
    PlayerComboAttackData::TriggerCondition condition,
    PlayerComboAttackController* attackController) {

    const auto& allAttacks = attackController->GetAllAttacks();

    for (const auto& atk : allAttacks) {
        if (!atk) {
            continue;
        }
        if (!attackController->IsFirstAttack(atk->GetGroupName())) {
            continue;
        }
        if (atk->GetAttackParam().triggerParam.condition != condition) {
            continue;
        }

        std::vector<ComboPathBuilder::ComboStep> currentPath;
        std::unordered_set<std::string> visited;

        ComboPathBuilder::ComboStep firstStep;
        firstStep.gamepadButton = atk->GetAttackParam().triggerParam.gamePadBottom;
        firstStep.isUnlocked    = atk->GetAttackParam().isUnlocked;
        firstStep.isAutoAdvance = atk->GetAttackParam().timingParam.isAutoAdvance;
        firstStep.attackName    = atk->GetGroupName();

        currentPath.push_back(firstStep);
        visited.insert(firstStep.attackName);

        if (DfsPath(atk.get(), unlockedAttackName, attackController, currentPath, visited)) {
            return currentPath;
        }
    }
    return {};
}

///==========================================================
/// 再帰DFS
///==========================================================
bool ComboUnlockNotifier::DfsPath(
    PlayerComboAttackData* current,
    const std::string& targetName,
    PlayerComboAttackController* attackController,
    std::vector<ComboPathBuilder::ComboStep>& currentPath,
    std::unordered_set<std::string>& visited) {

    if (current->GetGroupName() == targetName) {
        return true;
    }

    for (const auto& branch : current->GetComboBranches()) {
        const std::string& nextName = branch->GetNextAttackName();
        int32_t branchButton        = branch->GetGamepadButton();
        if (branchButton == 0 || nextName.empty()) {
            continue;
        }
        if (visited.count(nextName) > 0) {
            continue;
        }

        PlayerComboAttackData* nextAtk = attackController->GetAttackByName(nextName);
        if (!nextAtk) {
            continue;
        }

        ComboPathBuilder::ComboStep step;
        step.gamepadButton = branchButton;
        step.isUnlocked    = nextAtk->GetAttackParam().isUnlocked;
        step.isAutoAdvance = nextAtk->GetAttackParam().timingParam.isAutoAdvance;
        step.attackName    = nextName;

        currentPath.push_back(step);
        visited.insert(nextName);

        if (DfsPath(nextAtk, targetName, attackController, currentPath, visited)) {
            return true;
        }

        currentPath.pop_back();
        visited.erase(nextName);
    }
    return false;
}

///==========================================================
/// Condition → アイコンテクスチャパス解決
///==========================================================
const char* ComboUnlockNotifier::ResolveConditionIconPath(
    PlayerComboAttackData::TriggerCondition condition) {

    (void)condition;
    return nullptr;
}

///==========================================================
/// 背景・テキストスプライト初期化
///==========================================================
void ComboUnlockNotifier::InitBackSprite() {

    // 背景スプライト
    backgroundSprite.reset(KetaEngine::Sprite::Create("OperateUI/NotifyBackground.dds", true));
    if (backgroundSprite) {
        backgroundSprite->transform_.scale = {1.0f, 0.0f};
    }

    // テキストスプライト
    notifierTextSprite.reset(KetaEngine::Sprite::Create("OperateUI/NotifyText.dds", true));
    if (notifierTextSprite) {
        notifierTextSprite->transform_.scale = {1.0f, 0.0f};
    }
}

///==========================================================
/// パスと Condition からUI一式を生成してカードに詰める
///==========================================================
void ComboUnlockNotifier::PopulateCard(
    NotifyCard& card,
    const std::vector<ComboPathBuilder::ComboStep>& steps,
    const LayoutParam& layoutParam,
    int32_t cardIndex,
    PlayerComboAttackData::TriggerCondition condition) {

    const int32_t kLayerNum = 35;
    const int32_t kRow      = 0;

    // 通知UI専用レイアウト
    LayoutParam notifyLayout  = layoutParam;
    notifyLayout.basePosition = notifyBasePosition_;
    notifyLayout.basePosition.y += cardIndex * cardSpacingY_;
    notifyLayout.branchYOffset = 0.0f;
    notifyLayout.columnSpacing = spaceColumn_;
    notifyLayout.arrowOffset   = arrowOffsetPos_;
    int32_t col                = 0;

    //------------------------------------------------------
    //  Condition アイコン
    //------------------------------------------------------
    const char* iconPath = ResolveConditionIconPath(condition);
    if (iconPath) {
        card.conditionIconSprite.reset(KetaEngine::Sprite::Create(iconPath, false));
        if (card.conditionIconSprite) {
            card.conditionIconBaseScaleY = notifyLayout.buttonScale;
            card.conditionIconSprite->SetLayerNum(kLayerNum);
            card.conditionIconSprite->SetAnchorPoint({0.5f, 0.5f});
            card.conditionIconSprite->transform_.scale = {notifyLayout.buttonScale, 0.0f};
            card.conditionIconSprite->transform_.pos   = {
                notifyLayout.basePosition.x + col * notifyLayout.columnSpacing,
                notifyLayout.basePosition.y};
        }
        ++col;

        auto arrow = std::make_unique<ComboAsistArrowUI>();
        arrow->Init(col - 1, kRow, col, kRow, notifyLayout);
        arrow->SnapToTarget();
        arrow->SetExtraScale(buttonExtraScale_);
        arrow->SetScaleY(0.0f);
        card.arrowUIs.push_back(std::move(arrow));
    }

    //------------------------------------------------------
    //  攻撃ステップ（始点〜解放ステップ）
    //------------------------------------------------------
    bool isFirstButton = true;
    for (size_t i = 0; i < steps.size(); ++i) {
        if (i > 0 && steps[i - 1].isAutoAdvance) {
            continue;
        }

        if (!isFirstButton || !iconPath) {
            if (col > 0) {
                auto arrow = std::make_unique<ComboAsistArrowUI>();
                arrow->Init(col - 1, kRow, col, kRow, notifyLayout);
                arrow->SnapToTarget();
                arrow->SetExtraScale(buttonExtraScale_);
                arrow->SetScaleY(0.0f);
                card.arrowUIs.push_back(std::move(arrow));
            }
        }

        const auto& step = steps[i];
        auto btn         = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, step.isUnlocked, notifyLayout, step.attackName);
        btn->SetRowColumn(kRow, col);
        btn->ApplyLayout();
        btn->SetExtraScale(buttonExtraScale_);

        if (i == steps.size() - 1) {
            btn->SetActiveOutLine(true);
        }

        btn->SnapToTarget();
        btn->SetScaleY(0.0f);
        card.buttonUIs.push_back(std::move(btn));

        isFirstButton = false;
        ++col;
    }
}

///==========================================================
/// カード再配置
///==========================================================
void ComboUnlockNotifier::RearrangeCards() {
    for (size_t i = 0; i < cards_.size(); ++i) {
        if (!cards_[i]) {
            continue;
        }

        float targetY = notifyBasePosition_.y + static_cast<float>(i) * cardSpacingY_;

        if (cards_[i]->conditionIconSprite) {
            cards_[i]->conditionIconSprite->transform_.pos.y = targetY;
        }

        for (auto& btn : cards_[i]->buttonUIs) {
            if (btn) {
                btn->SetTargetPosY(targetY);
            }
        }
        for (auto& arrow : cards_[i]->arrowUIs) {
            if (arrow) {
                arrow->SetTargetPosY(targetY);
            }
        }
    }
}


void ComboUnlockNotifier::RegisterParams() {
    globalParameter_->Regist(groupName_, "NotifyBasePositionX", &notifyBasePosition_.x);
    globalParameter_->Regist(groupName_, "NotifyBasePositionY", &notifyBasePosition_.y);
    globalParameter_->Regist(groupName_, "CardSpacingY", &cardSpacingY_);
    globalParameter_->Regist(groupName_, "ButtonExtraScale", &buttonExtraScale_);
    globalParameter_->Regist(groupName_, "spaceColumn", &spaceColumn_);
    globalParameter_->Regist(groupName_, "arrowOffsetPos", &arrowOffsetPos_);
}

///==========================================================
/// ImGui調整
///==========================================================
void ComboUnlockNotifier::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("ComboUnlockNotifier")) {
        ImGui::DragFloat2("NotifyBasePosition", &notifyBasePosition_.x, 1.0f);
        ImGui::DragFloat("CardSpacingY", &cardSpacingY_, 1.0f);
        ImGui::DragFloat("ButtonExtraScale", &buttonExtraScale_, 0.01f, 0.1f, 5.0f);
        ImGui::DragFloat("spaceColumn", &spaceColumn_, 0.1f);
        ImGui::DragFloat2("矢印オフセット位置", &arrowOffsetPos_.x, 0.1f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }
#endif
}
