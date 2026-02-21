#pragma once
// AsistParts
#include "UI/ComboAsistUI/AsistParts/ComboAsistButtonUI.h"
#include "UI/ComboAsistUI/AsistParts/ComboAsistArrowUI.h"
// Builder / Data
#include "../ComboPathBuilder.h"
#include "../ComboUIBuilder.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackData.h"
// Easing
#include "Easing/Easing.h"
// Sprite
#include "2d/Sprite.h"
// std
#include <memory>
#include <string>
#include <vector>
#include <functional>


class PlayerComboAttackController;

/// <summary>
/// コンボ解放通知UI
/// </summary>
class ComboUnlockNotifier {
public:
    /// 1枚の通知カード
    struct NotifyCard {
        // Conditionのアイコン用スプライト
        std::unique_ptr<KetaEngine::Sprite> conditionIconSprite;

        // 攻撃ステップのボタンUI群
        std::vector<std::unique_ptr<ComboAsistButtonUI>> buttonUIs;

        // ボタン間の矢印UI
        std::vector<std::unique_ptr<ComboAsistArrowUI>> arrowUIs;

        float lifetime    = 0.0f;
        float fadeOutTime = 0.0f;
        bool  isFadingOut = false;

        KetaEngine::Easing<float> slideInEasing;
        float slideOffsetX = 0.0f;
        bool  isSliding    = false;

        // フェードインアウト
        std::function<void(NotifyCard&, float)> updateFunc;
    };

public:
    ComboUnlockNotifier()  = default;
    ~ComboUnlockNotifier() = default;

    /// 毎フレーム呼ぶ
    void Update(float deltaTime);

    /// 攻撃が解放されたときに外部から呼ぶ
    void OnAttackUnlocked(
        const std::string&           unlockedAttackName,
        const LayoutParam&           layoutParam,
        PlayerComboAttackController* attackController);

private:
    /// Condition に対応するアイコンのテクスチャパスを返す
     const char* ResolveConditionIconPath(PlayerComboAttackData::TriggerCondition condition);
    void UpdateCardDisplay(NotifyCard& card, float deltaTime);
    void UpdateCardFadeOut(NotifyCard& card, float deltaTime);
    
    /// 解放された攻撃を始点まで遡り直線パスを構築する
    std::vector<ComboPathBuilder::ComboStep> BuildLinearPath(
        const std::string&                      unlockedAttackName,
        PlayerComboAttackData::TriggerCondition condition,
        PlayerComboAttackController*            attackController);

     bool DfsPath(
        PlayerComboAttackData* current,
        const std::string& targetName,
        PlayerComboAttackController* attackController,
        std::vector<ComboPathBuilder::ComboStep>& currentPath,
        std::unordered_set<std::string>& visited);

    /// パスと Condition からUI一式を生成してカードに詰める
    void PopulateCard(
        NotifyCard&                                        card,
        const std::vector<ComboPathBuilder::ComboStep>&   steps,
        const LayoutParam&                                 layoutParam,
        int32_t                                            cardIndex,
        PlayerComboAttackData::TriggerCondition            condition);

    void ApplyAlphaToCard(NotifyCard& card, float alpha);
    void ApplySlideToCard(NotifyCard& card);
    void RearrangeCards();

private:
    std::vector<std::unique_ptr<NotifyCard>> cards_;

    Vector2     notifyBasePosition_  = {800.0f, 400.0f};
    float       displayTime_         = 3.0f;
    float       fadeOutTimeDuration_ = 0.8f;
    float       cardSpacingY_        = 120.0f;
    std::string slideInEasingFile_   = "ComboAsistSlideIn.json";

public:
    size_t GetActiveCardCount() const { return cards_.size(); }

    void SetNotifyBasePosition(const Vector2& pos) { notifyBasePosition_ = pos; }
    void SetDisplayTime(float sec) { displayTime_ = sec; }
    void SetFadeOutTime(float sec) { fadeOutTimeDuration_ = sec; }
    void SetCardSpacingY(float spacing) { cardSpacingY_ = spacing; }
    void SetSlideInEasingFile(const std::string& f) { slideInEasingFile_ = f; }
};
