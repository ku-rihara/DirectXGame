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
// GlobalParameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// Sprite
#include "2d/Sprite.h"
// std
#include <memory>
#include <string>
#include <vector>

class PlayerComboAttackController;
class Player;

/// <summary>
/// コンボ解放通知UI
/// </summary>
class ComboUnlockNotifier {
public:
    /// 1枚の通知カード
    struct NotifyCard {
        // Conditionのアイコン用スプライト
        std::unique_ptr<KetaEngine::Sprite> conditionIconSprite;
        float conditionIconBaseScaleY = 1.0f;

        // 攻撃ステップのボタンUI群
        std::vector<std::unique_ptr<ComboAsistButtonUI>> buttonUIs;

        // ボタン間の矢印UI
        std::vector<std::unique_ptr<ComboAsistArrowUI>> arrowUIs;

        // 状態
        enum class State { OPENING, DISPLAYING, CLOSING, DONE };
        State state = State::OPENING;

        // スケールYイージング（0→1 Open, 1→0 Close）
        KetaEngine::Easing<float> scaleYEasing;
        float scaleY = 0.0f;

        // 自動実行用データ
        Player* player = nullptr;
        std::vector<PlayerComboAttackData*> pendingAttacks;
        int32_t totalAttackCount    = 0;
        int32_t executedAttackCount = 0;
    };

public:
    ComboUnlockNotifier()  = default;
    ~ComboUnlockNotifier() = default;

    /// 初期化（GlobalParameter登録 & EasingCreatorロード）
    void Init();

    /// 毎フレーム呼ぶ
    void Update(float deltaTime);

    /// 攻撃が解放されたときに外部から呼ぶ
    void OnAttackUnlocked(
        const std::string&           unlockedAttackName,
        const LayoutParam&           layoutParam,
        PlayerComboAttackController* attackController,
        Player*                      player = nullptr);

    /// 攻撃が自動実行されたときに呼ぶ（通知UIにリアクションさせる）
    void NotifyAttackExecuted(const std::string& attackName);

    /// ImGui編集（EasingCreator + GlobalParameter）
    void AdjustParam();

private:
    /// Condition に対応するアイコンのテクスチャパスを返す
    const char* ResolveConditionIconPath(PlayerComboAttackData::TriggerCondition condition);

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

    /// カードの状態を更新
    void UpdateCardState(NotifyCard& card, float deltaTime);

    /// スケールYをカードのUI全体に適用
    void ApplyScaleYToCard(NotifyCard& card);

    /// クローズアニメーション開始
    void StartCloseAnimation(NotifyCard& card);

    void RearrangeCards();

private:
    std::vector<std::unique_ptr<NotifyCard>> cards_;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_          = "ComboUnlockNotifier";
    const std::string kScaleYEasingFile_  = "ComboUnlockOpen.json";

    Vector2 notifyBasePosition_ = {800.0f, 400.0f};
    float   cardSpacingY_       = 120.0f;

public:
    size_t GetActiveCardCount() const { return cards_.size(); }

    void SetNotifyBasePosition(const Vector2& pos) { notifyBasePosition_ = pos; }
    void SetCardSpacingY(float spacing) { cardSpacingY_ = spacing; }
};
