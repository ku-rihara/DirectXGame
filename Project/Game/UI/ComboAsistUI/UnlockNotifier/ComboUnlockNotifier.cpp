#include "ComboUnlockNotifier.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include <algorithm>
#include <cmath>
#include <functional>

void ComboUnlockNotifier::Update(float deltaTime) {
    for (auto& card : cards_) {
        if (!card) {
            continue;
        }

        // スライドインの更新
        if (card->isSliding) {
            card->slideInEasing.Update(deltaTime);
            // 終了処理
            if (card->slideInEasing.IsFinished()) {
                card->isSliding    = false;
                card->slideOffsetX = 0.0f;
            }
            ApplySlideToCard(*card);
        }

        // 状態別更新（表示中 / フェードアウト中）
        card->updateFunc(*card, deltaTime);

        // フェードアウトが終わったカードを消滅
        if (card->isFadingOut && card->fadeOutTime <= 0.0f) {
            card.reset();
        }
    }

    // 消滅済みカードを除去
    cards_.erase(
        std::remove_if(cards_.begin(), cards_.end(),
            [](const std::unique_ptr<NotifyCard>& c) { return !c; }),
        cards_.end());

    RearrangeCards();

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

void ComboUnlockNotifier::UpdateCardDisplay(NotifyCard& card, float deltaTime) {
    card.lifetime -= deltaTime;
    if (card.lifetime <= 0.0f) {
        card.lifetime    = 0.0f;
        card.isFadingOut = true;
        card.fadeOutTime = fadeOutTimeDuration_;
        card.updateFunc  = [this](NotifyCard& c, float dt) {
            UpdateCardFadeOut(c, dt);
        };
    }
}

void ComboUnlockNotifier::UpdateCardFadeOut(NotifyCard& card, float deltaTime) {
    card.fadeOutTime -= deltaTime;
    float alpha = (std::max)(0.0f, card.fadeOutTime / fadeOutTimeDuration_);
    ApplyAlphaToCard(card, alpha);
}

void ComboUnlockNotifier::OnAttackUnlocked(
    const std::string& unlockedAttackName,
    const LayoutParam& layoutParam,
    PlayerComboAttackController* attackController) {

    if (!attackController) {
        return;
    }

    // 攻撃を取得
    PlayerComboAttackData* unlockedData = attackController->GetAttackByName(unlockedAttackName);

    if (!unlockedData) {
        return;
    }

    // ConditionTypeを取得
    auto condition = unlockedData->GetAttackParam().triggerParam.condition;

    // ComboStepのvector
    auto steps = BuildLinearPath(unlockedAttackName, condition, attackController);

    // 早期リターン
    if (steps.empty()) {
        return;
    }

    // NotifyCard　初期化
    auto card         = std::make_unique<NotifyCard>();
    card->lifetime    = displayTime_;
    card->fadeOutTime = fadeOutTimeDuration_;
    card->isFadingOut = false;

    // 初期状態は「表示中」の更新処理
    card->updateFunc = [this](NotifyCard& c, float dt) {
        UpdateCardDisplay(c, dt);
    };

    // スライドイージングの初期化
    card->slideInEasing.Init(slideInEasingFile_);
    card->slideInEasing.SetAdaptValue(&card->slideOffsetX);
    card->slideInEasing.Reset();
    card->isSliding = true;

    int32_t cardIndex = static_cast<int32_t>(cards_.size());
    PopulateCard(*card, steps, layoutParam, cardIndex, condition);
    cards_.push_back(std::move(card));
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

        // バックトラック
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

    using TC = PlayerComboAttackData::TriggerCondition;
    switch (condition) {
    case TC::AIR:
        return "OperateUI/ConditionAir.dds";
    case TC::DASH:
        return "OperateUI/ConditionDash.dds";
    case TC::GROUND:
    case TC::BOTH:
    default:
        return nullptr;
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

    constexpr int32_t kLayerNum = 20;
    constexpr int32_t kRow      = 0;

    // 通知UI専用レイアウト
    LayoutParam notifyLayout  = layoutParam;
    notifyLayout.basePosition = notifyBasePosition_;
    notifyLayout.basePosition.y += cardIndex * cardSpacingY_;
    notifyLayout.branchYOffset = 0.0f;

    int32_t col = 0;

    //------------------------------------------------------
    // ① Condition アイコン
    //------------------------------------------------------
    const char* iconPath = ResolveConditionIconPath(condition);
    if (iconPath) {
        card.conditionIconSprite.reset(KetaEngine::Sprite::Create(iconPath, false));
        if (card.conditionIconSprite) {
            card.conditionIconSprite->SetLayerNum(kLayerNum);
            card.conditionIconSprite->SetAnchorPoint({0.5f, 0.5f});
            card.conditionIconSprite->transform_.scale = {notifyLayout.buttonScale, notifyLayout.buttonScale};
            card.conditionIconSprite->transform_.pos   = {
                notifyLayout.basePosition.x + col * notifyLayout.columnSpacing + card.slideOffsetX,
                notifyLayout.basePosition.y};
        }
        ++col;

        // アイコン → 始点ボタン間の矢印
        auto arrow = std::make_unique<ComboAsistArrowUI>();
        arrow->Init(col - 1, kRow, col, kRow, notifyLayout);
        arrow->SnapToTarget();
        card.arrowUIs.push_back(std::move(arrow));
    }

    //------------------------------------------------------
    // ② 攻撃ステップ（始点〜解放ステップ）
    //------------------------------------------------------
    bool isFirstButton = true;
    for (size_t i = 0; i < steps.size(); ++i) {
        if (i > 0 && steps[i - 1].isAutoAdvance) {
            continue;
        }

        // アイコンがある場合、最初のボタン前の矢印は①で追加済みなのでスキップ
        if (!isFirstButton || !iconPath) {
            if (col > 0) {
                auto arrow = std::make_unique<ComboAsistArrowUI>();
                arrow->Init(col - 1, kRow, col, kRow, notifyLayout);
                arrow->SnapToTarget();
                card.arrowUIs.push_back(std::move(arrow));
            }
        }

        const auto& step = steps[i];
        auto btn         = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, step.isUnlocked, kRow, col, notifyLayout);

        // 最後（解放されたステップ）をアクティブアウトラインで強調
        if (i == steps.size() - 1) {
            btn->SetActiveOutLine(true);
        }

        btn->SnapToTarget();
        card.buttonUIs.push_back(std::move(btn));

        isFirstButton = false;
        ++col;
    }
}

///==========================================================
/// アルファ適用（フェードアウト用）
///==========================================================
void ComboUnlockNotifier::ApplyAlphaToCard(NotifyCard& card, float alpha) {
    if (card.conditionIconSprite) {
        card.conditionIconSprite->SetAlpha(alpha);
    }
  /*  for (auto& btn : card.buttonUIs) {
        if (btn)
            btn->SetAlpha(alpha);
    }
    for (auto& arrow : card.arrowUIs) {
        if (arrow)
            arrow->SetAlpha(alpha);
    }*/
}

///==========================================================
/// スライドオフセット適用
///==========================================================
void ComboUnlockNotifier::ApplySlideToCard(NotifyCard& card) {
    if (card.conditionIconSprite) {
        // col=0 の X 座標にスライドオフセットを加算
        card.conditionIconSprite->transform_.pos.x =
            notifyBasePosition_.x + card.slideOffsetX;
    }
    for (auto& btn : card.buttonUIs) {
        if (btn)
            btn->ApplySlideOffset(card.slideOffsetX);
    }
    for (auto& arrow : card.arrowUIs) {
        if (arrow)
            arrow->ApplySlideOffset(card.slideOffsetX);
    }
}


void ComboUnlockNotifier::RearrangeCards() {
    for (size_t i = 0; i < cards_.size(); ++i) {
        if (!cards_[i]) {
            continue;
        }

        float targetY = notifyBasePosition_.y + static_cast<float>(i) * cardSpacingY_;

        // conditionIconSprite の Y を更新
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