#pragma once
// AsistParts
#include "UI/ComboAsistUI/AsistParts/ComboAsistArrowUI.h"
#include "UI/ComboAsistUI/AsistParts/ComboAsistButtonUI.h"
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
        enum class State {
            OPENING,
            DISPLAYING,
            CLOSE_WAITING, //< クローズ前の待機
            CLOSING,
            DONE
        };

        State state             = State::OPENING;
        float closeWaitTimer    = 0.0f;

        // 自動実行用データ
        Player* player = nullptr;
        std::vector<PlayerComboAttackData*> pendingAttacks;
        int32_t totalAttackCount    = 0;
        int32_t executedAttackCount = 0;
    };

public:
    ComboUnlockNotifier()  = default;
    ~ComboUnlockNotifier() = default;

    /// 初期化
    void Init();

    /// 毎フレーム呼ぶ
    void Update(float deltaTime);

    /// 攻撃が解放されたときに外部から呼ぶ
    void OnAttackUnlocked(
        const std::string& unlockedAttackName,
        const LayoutParam& layoutParam,
        PlayerComboAttackController* attackController,
        Player* player = nullptr);

    /// 攻撃が自動実行されたときに呼ぶ
    void NotifyAttackExecuted(const std::string& attackName);

    /// ImGui編集
    void AdjustParam();

private:
    /// Condition に対応するアイコンのテクスチャパスを返す
    const char* ResolveConditionIconPath(PlayerComboAttackData::TriggerCondition condition);

    /// 解放された攻撃を始点まで遡り直線パスを構築する
    std::vector<ComboPathBuilder::ComboStep> BuildLinearPath(
        const std::string& unlockedAttackName,
        PlayerComboAttackData::TriggerCondition condition,
        PlayerComboAttackController* attackController);

    bool DfsPath(
        PlayerComboAttackData* current,
        const std::string& targetName,
        PlayerComboAttackController* attackController,
        std::vector<ComboPathBuilder::ComboStep>& currentPath,
        std::unordered_set<std::string>& visited);

    /// カードの背景・テキストスプライトを初期化する
    void InitBackSprite();

    /// パスと Condition からUI一式を生成してカードに詰める
    void PopulateCard(
        NotifyCard& card,
        const std::vector<ComboPathBuilder::ComboStep>& steps,
        const LayoutParam& layoutParam,
        int32_t cardIndex,
        PlayerComboAttackData::TriggerCondition condition);

    /// カードの状態を更新
    void UpdateCardState(NotifyCard& card, float deltaTime);

    /// スケールYをカードのUI全体に適用
    void ApplyScaleYToCard(NotifyCard& card);

    /// クローズアニメーション開始
    void StartCloseAnimation(NotifyCard& card);

    void RearrangeCards();

    // パラメータ登録
    void RegisterParams();

private:
    std::vector<std::unique_ptr<NotifyCard>> cards_;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "ComboUnlockNotifier";
    const std::string kScaleYEasingFile_          = "ComboUnlockOpen.json";

    Vector2 notifyBasePosition_;
    float cardSpacingY_;

    /// スケールYイージング
    KetaEngine::Easing<float> scaleYEasing_;
    float scaleY_ = 0.0f;

    // 背景スプライト
    std::unique_ptr<KetaEngine::Sprite> backgroundSprite;
    // テキストスプライト
    std::unique_ptr<KetaEngine::Sprite> notifierTextSprite;

    /// ボタン・矢印UIに追加で掛けるスケール倍率
    float buttonExtraScale_  = 1.0f;
    float spaceColumn_       = 1.0f;
    float closeOffsetTime_   = 1.0f; //< 最後の演出後、閉じるまでの待機時間
    Vector2 arrowOffsetPos_;

public:
    size_t GetActiveCardCount() const { return cards_.size(); }

    void SetNotifyBasePosition(const Vector2& pos) { notifyBasePosition_ = pos; }
    void SetCardSpacingY(float spacing) { cardSpacingY_ = spacing; }
};