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

    // 初期状態は画面右端外（通知前は非表示と同等の位置）
    slideNorm_    = 1.0f;
    slideOffsetX_ = slideStartOffsetX_;
    ApplySlideToShared();
}

///==========================================================
/// 毎フレーム更新
///==========================================================
void ComboUnlockNotifier::Update(float deltaTime) {
    // スライドオフセット計算
    slideOffsetX_ = slideNorm_ * slideStartOffsetX_;

    for (auto& card : cards_) {
        if (!card) {
            continue;
        }

        UpdateCardState(*card, deltaTime);
        ApplySlideToCard(*card);

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

    // 共有スプライト（背景・テキスト）のスライド適用
    ApplySlideToShared();

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
        slideEasing_.Update(deltaTime);
        if (slideEasing_.IsFinished()) {
            // スライドイン完了：最後のボタンにアンロック演出を再生（通知UIなので音なし）
            if (card.unlockTargetButton) {
                card.unlockTargetButton->SetUnlockSoundEnabled(false);
                card.unlockTargetButton->SetUnlocked(true);
            }
            card.state          = NotifyCard::State::CLOSE_WAITING;
            card.closeWaitTimer = 0.0f;

            /* --- 自動再生（一旦オフ） ---
            card.state = NotifyCard::State::DISPLAYING;
            if (card.player) {
                auto& queue = card.player->GetAutoComboQueue();
                queue.Clear();
                for (auto* data : card.pendingAttacks) {
                    queue.Enqueue(data);
                }
            }
            if (card.totalAttackCount == 0) {
                card.state          = NotifyCard::State::CLOSE_WAITING;
                card.closeWaitTimer = 0.0f;
            }
            */
        }
        break;
    case NotifyCard::State::DISPLAYING:
        // 自動再生オフのため、このステートには遷移しないが念のためクローズ待機へ
        card.state          = NotifyCard::State::CLOSE_WAITING;
        card.closeWaitTimer = 0.0f;
        break;
    case NotifyCard::State::CLOSE_WAITING:
        card.closeWaitTimer += deltaTime;
        if (card.closeWaitTimer >= closeOffsetTime_) {
            StartSlideOut(card);
        }
        break;
    case NotifyCard::State::CLOSING:
        slideEasing_.Update(deltaTime);
        if (slideEasing_.IsFinished()) {
            card.state = NotifyCard::State::DONE;
        }
        break;
    case NotifyCard::State::DONE:
        break;
    }
}

///==========================================================
/// カードのUI要素にスライドオフセットを適用
///==========================================================
void ComboUnlockNotifier::ApplySlideToCard(NotifyCard& card) {
    // conditionIconはbasePositionXのcol=0位置
    if (card.conditionIconSprite) {
        card.conditionIconSprite->transform_.pos.x = card.basePositionX + slideOffsetX_;
    }

    for (auto& btn : card.buttonUIs) {
        if (btn) {
            btn->ApplySlideOffset(slideOffsetX_);
        }
    }
    for (auto& arrow : card.arrowUIs) {
        if (arrow) {
            arrow->ApplySlideOffset(slideOffsetX_);
        }
    }
}

///==========================================================
/// 共有スプライト（背景・テキスト）のスライド適用
///==========================================================
void ComboUnlockNotifier::ApplySlideToShared() {
    if (backgroundSprite) {
        backgroundSprite->transform_.pos = {
            notifyBasePosition_.x + bgSpriteOffset_.x + slideOffsetX_,
            notifyBasePosition_.y + bgSpriteOffset_.y};
        backgroundSprite->transform_.scale = bgSpriteScale_;
    }
    if (notifierTextSprite) {
        notifierTextSprite->transform_.pos = {
            notifyBasePosition_.x + textSpriteOffset_.x + slideOffsetX_,
            notifyBasePosition_.y + textSpriteOffset_.y};
        notifierTextSprite->transform_.scale = textSpriteScale_;
    }
}

///==========================================================
/// スライドアウト開始（0→1: 定位置→画面右端外）
///==========================================================
void ComboUnlockNotifier::StartSlideOut(NotifyCard& card) {
    slideEasing_.SetIsStartEndReverse(false); // 0→1（スライドアウト）
    slideEasing_.SetAdaptValue(&slideNorm_);
    slideEasing_.Reset();
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

    // 将来の自動実行再有効化のためデータを収集（現在は使用しない）
    if (player) {
        for (const auto& step : steps) {
            PlayerComboAttackData* data = attackController->GetAttackByName(step.attackName);
            if (data) {
                card->pendingAttacks.push_back(data);
            }
        }
        card->totalAttackCount = static_cast<int32_t>(card->pendingAttacks.size());
    }

    // スライドイージング設定（1→0: 画面外→定位置）
    slideEasing_.Init(kSlideEasingFile_);
    slideEasing_.SetIsStartEndReverse(true); // 1→0（スライドイン）
    slideEasing_.SetAdaptValue(&slideNorm_);
    slideNorm_ = 1.0f; // 初期状態は画面外（右端）
    slideEasing_.Reset();

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
            card->state          = NotifyCard::State::CLOSE_WAITING;
            card->closeWaitTimer = 0.0f;
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
        backgroundSprite->transform_.scale = bgSpriteScale_;
    }

    // テキストスプライト
    notifierTextSprite.reset(KetaEngine::Sprite::Create("OperateUI/NotifyText.dds", true));
    if (notifierTextSprite) {
        notifierTextSprite->transform_.scale = textSpriteScale_;
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

    //------------------------------------------------------
    // 1. 総列数を先行カウントしてbasePosition.xを算出
    //------------------------------------------------------
    const char* iconPath = ResolveConditionIconPath(condition);
    int32_t colCount     = 0;
    if (iconPath) {
        colCount++; // condition icon
    }
    for (size_t i = 0; i < steps.size(); ++i) {
        if (i > 0 && steps[i - 1].isAutoAdvance) {
            continue;
        }
        colCount++;
    }
    // 最後の列インデックス = colCount - 1
    // 最後の列がrightAnchorX_に来るようにbasePosition.xを算出
    const float adjustedBaseX =
        (colCount > 0)
            ? rightAnchorX_ - static_cast<float>(colCount - 1) * spaceColumn_
            : rightAnchorX_;

    //------------------------------------------------------
    // 2. 通知UI専用レイアウト設定
    //------------------------------------------------------
    LayoutParam notifyLayout  = layoutParam;
    notifyLayout.basePosition = notifyBasePosition_;
    notifyLayout.basePosition.x = adjustedBaseX;
    notifyLayout.basePosition.y += cardIndex * cardSpacingY_;
    notifyLayout.yGroupOffsetY = 0.0f;
    notifyLayout.columnSpacing = spaceColumn_;
    notifyLayout.arrowOffset   = arrowOffsetPos_;

    // カードの基準X位置を保存
    card.basePositionX = adjustedBaseX;

    int32_t col = 0;

    //------------------------------------------------------
    // 3. Condition アイコン
    //------------------------------------------------------
    if (iconPath) {
        card.conditionIconSprite.reset(KetaEngine::Sprite::Create(iconPath, false));
        if (card.conditionIconSprite) {
            card.conditionIconSprite->SetLayerNum(kLayerNum);
            card.conditionIconSprite->SetAnchorPoint({0.5f, 0.5f});
            card.conditionIconSprite->transform_.scale = {notifyLayout.buttonScale, notifyLayout.buttonScale};
            card.conditionIconSprite->transform_.pos   = {
                notifyLayout.basePosition.x + col * notifyLayout.columnSpacing + slideStartOffsetX_,
                notifyLayout.basePosition.y};
        }
        ++col;

        auto arrow = std::make_unique<ComboAsistArrowUI>();
        arrow->Init(col - 1, kRow, col, kRow, notifyLayout);
        arrow->ApplySlideOffset(slideStartOffsetX_);
        arrow->SnapToTarget();
        arrow->SetExtraScale(buttonExtraScale_);
        card.arrowUIs.push_back(std::move(arrow));
    }

    //------------------------------------------------------
    // 4. 攻撃ステップ（始点〜解放ステップ）
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
                arrow->ApplySlideOffset(slideStartOffsetX_);
                arrow->SnapToTarget();
                arrow->SetExtraScale(buttonExtraScale_);
                card.arrowUIs.push_back(std::move(arrow));
            }
        }

        const auto& step    = steps[i];
        const bool isLast   = (i == steps.size() - 1);
        // 最後のボタン（新規開放）はロック状態で生成し、スライドイン後に演出を再生する
        const bool initUnlocked = isLast ? false : step.isUnlocked;

        auto btn = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, initUnlocked, notifyLayout, step.attackName);
        btn->SetRowColumn(kRow, col);
        btn->ApplyLayout();
        btn->SetExtraScale(buttonExtraScale_);

        if (isLast) {
            btn->SetActiveOutLine(true);
            card.unlockTargetButton = btn.get();
        }

        // 初期位置を画面外（スライドイン前）に設定
        btn->ApplySlideOffset(slideStartOffsetX_);
        btn->SnapToTarget();
        card.buttonUIs.push_back(std::move(btn));

        isFirstButton = false;
        ++col;
    }
}

///==========================================================
/// カード再配置（Y位置のみ）
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

///==========================================================
/// パラメータ登録
///==========================================================
void ComboUnlockNotifier::RegisterParams() {
    globalParameter_->Regist(groupName_, "NotifyBasePositionX", &notifyBasePosition_.x);
    globalParameter_->Regist(groupName_, "NotifyBasePositionY", &notifyBasePosition_.y);
    globalParameter_->Regist(groupName_, "CardSpacingY", &cardSpacingY_);
    globalParameter_->Regist(groupName_, "ButtonExtraScale", &buttonExtraScale_);
    globalParameter_->Regist(groupName_, "spaceColumn", &spaceColumn_);
    globalParameter_->Regist(groupName_, "arrowOffsetPos", &arrowOffsetPos_);
    globalParameter_->Regist(groupName_, "CloseOffsetTime", &closeOffsetTime_);
    globalParameter_->Regist(groupName_, "RightAnchorX", &rightAnchorX_);
    globalParameter_->Regist(groupName_, "SlideStartOffsetX", &slideStartOffsetX_);
    globalParameter_->Regist(groupName_, "BgSpriteScale", &bgSpriteScale_);
    globalParameter_->Regist(groupName_, "TextSpriteScale", &textSpriteScale_);
    globalParameter_->Regist(groupName_, "BgSpriteOffset", &bgSpriteOffset_);
    globalParameter_->Regist(groupName_, "TextSpriteOffset", &textSpriteOffset_);
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
        ImGui::DragFloat("CloseOffsetTime", &closeOffsetTime_, 0.1f, 0.0f, 10.0f);
        ImGui::Separator();
        ImGui::DragFloat("RightAnchorX（最右列ボタンX）", &rightAnchorX_, 1.0f);
        ImGui::DragFloat("SlideStartOffsetX（スライド幅）", &slideStartOffsetX_, 1.0f, 0.0f);
        ImGui::Separator();
        ImGui::DragFloat2("BgSpriteScale", &bgSpriteScale_.x, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat2("BgSpriteOffset", &bgSpriteOffset_.x, 1.0f);
        ImGui::DragFloat2("TextSpriteScale", &textSpriteScale_.x, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat2("TextSpriteOffset", &textSpriteOffset_.x, 1.0f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }
#endif
}
