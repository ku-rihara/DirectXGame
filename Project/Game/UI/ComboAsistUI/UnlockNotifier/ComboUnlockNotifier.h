#pragma once
// AsistParts
#include "UI/ComboAsistUI/AsistParts/ComboAsistArrowUI.h"
#include "UI/ComboAsistUI/AsistParts/ComboAsistButtonUI.h"
// Builder / Data
#include "../AsistBuilder/ComboPathBuilder.h"
#include "../AsistBuilder/ComboUIBuilder.h"
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
/// 右端からスライドインで出現し、一定時間後に右端へスライドアウトする
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

        /// このカードのレイアウト基準
        float basePositionX = 0.0f;

        // 状態
        enum class State {
            OPENING,
            DISPLAYING,
            CLOSE_WAITING, //< クローズ前の待機
            CLOSING,
            DONE
        };

        State state          = State::OPENING;
        float closeWaitTimer = 0.0f;

        // 将来の自動実行再有効化のためデータを保持
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

    /// カードのUI要素にスライドオフセットを適用（ボタン・矢印・conditionIcon）
    void ApplySlideToCard(NotifyCard& card);

    /// 共有スプライト（背景・テキスト）にスライドオフセットを適用
    void ApplySlideToShared();

    /// スライドアウトアニメーション開始
    void StartSlideOut(NotifyCard& card);

    void RearrangeCards();

    // パラメータ登録
    void RegisterParams();

private:
    std::vector<std::unique_ptr<NotifyCard>> cards_;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "ComboUnlockNotifier";

    /// スライドイージング（0→1=スライドアウト方向, 1→0=スライドイン方向）
    KetaEngine::Easing<float> slideEasing_;
    float slideNorm_    = 0.0f; // 0=定位置, 1=画面右端外
    float slideOffsetX_ = 0.0f; // = slideNorm_ * slideStartOffsetX_

    const std::string kSlideEasingFile_ = "ComboUnlockOpen.json"; // 既存ファイルを再利用

    Vector2 notifyBasePosition_;
    float cardSpacingY_     = 0.0f;

    /// 最右列ボタンのX位置
    float rightAnchorX_;
    /// 画面外スライド量
    float slideStartOffsetX_;

    // 背景スプライト
    std::unique_ptr<KetaEngine::Sprite> backgroundSprite;
    // テキストスプライト
    std::unique_ptr<KetaEngine::Sprite> notifierTextSprite;

    /// 背景・テキストスプライトのスケール
    Vector2 bgSpriteScale_   = {1.0f, 1.0f};
    Vector2 textSpriteScale_ = {1.0f, 1.0f};
    /// 背景・テキストスプライトの位置オフセット（notifyBasePosition_からの相対位置）
    Vector2 bgSpriteOffset_;
    Vector2 textSpriteOffset_;

    /// ボタン・矢印UIに追加で掛けるスケール倍率
    float buttonExtraScale_ = 1.0f;
    float spaceColumn_      = 1.0f;
    float closeOffsetTime_  = 1.0f; //< 表示後、閉じるまでの待機時間
    Vector2 arrowOffsetPos_;

public:
    size_t GetActiveCardCount() const { return cards_.size(); }

    void SetNotifyBasePosition(const Vector2& pos) { notifyBasePosition_ = pos; }
    void SetCardSpacingY(float spacing) { cardSpacingY_ = spacing; }
};
