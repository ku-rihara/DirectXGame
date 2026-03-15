#pragma once
// UI生成・ヘルパー
#include "AsistBuilder/ComboUIBuilder.h"
// 分割クラス
#include "SubControllers/ComboAsistColumnScroller.h"
#include "SubControllers/ComboAsistConditionSwitcher.h"
#include "SubControllers/ComboAsistVisibilityController.h"
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// Easing
#include "Easing/Easing.h"
// math
#include "Vector2.h"
// std
#include <cstdint>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

class Player;
class PlayerComboAttackController;

/// <summary>
/// コンボアシストUI管理クラス
/// </summary>
class ComboAsistController {
public:
    enum class PanelMode {
        Close,
        Open };

    ComboAsistController()  = default;
    ~ComboAsistController() = default;

    void Init();
    void Update();
    void AdjustParam();

    //=== パネル開閉 ===
    void OpenPanel();
    void TogglePanelMode();

    //=== スライドイン・アウト ===
    void StartSlideIn();
    void StartSlideOut();
    void UpdateSlide(float deltaTime);
    void ApplySlideOffset();

private:
    //=== 初期化 ===
    void RebuildAllConditions();
    void RegisterParams();

    //=== スライド（内部用） ===
    void ApplySlideOffsetToCondition(ConditionUIData& data);

    //=== トグル入力 ===
    void CheckToggleInput();

    //=== 攻撃発動演出 ===
    void UpdateComboState();
    void SetGroupActiveOutLines(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup, const std::unordered_set<std::string>& activeAttacks);
    void PlayPushScalingForAttack(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup, const std::string& attackName);

    //=== ヘルパー ===
    LayoutParam MakeLayoutParam() const;
    ConditionUIData* GetCurrentData();

private:
    Player* pPlayer_                                = nullptr;
    PlayerComboAttackController* pAttackController_ = nullptr;
    KetaEngine::GlobalParameter* globalParameter_   = nullptr;
    std::string groupName_                          = "ComboAsistUI";

    // UI生成担当
    ComboUIBuilder uiBuilder_;

    // 分割クラス
    ComboAsistVisibilityController visibilityController_;
    ComboAsistColumnScroller columnScroller_;
    ComboAsistConditionSwitcher conditionSwitcher_;

    // 条件別UIデータ（全条件を事前構築）
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData> conditionDataMap_;
    std::vector<PlayerComboAttackData::TriggerCondition> availableConditions_;

    // 攻撃発動演出用
    std::unordered_set<std::string> playedAttacks_;
    std::string prevBehaviorName_;

    // スライドイン・アウト
    KetaEngine::Easing<float> slideInEasing_;
    KetaEngine::Easing<float> slideOutEasing_;
    KetaEngine::Easing<float>* activeSlideEasing_ = nullptr;

    float slideOffsetX_ = 0.0f;
    bool isVisible_     = false;
    bool isSliding_     = false;

    // パネル開閉モード
    PanelMode panelMode_      = PanelMode::Close;
    bool isModeTransitioning_ = false;

    // 表示範囲（minは常に0）
    int32_t maxVisibleColumn_ = 5;
    int32_t maxVisibleRow_    = 4;

    // 発動条件
    PlayerComboAttackData::TriggerCondition currentCondition_ = PlayerComboAttackData::TriggerCondition::GROUND;

    // レイアウトパラメータ
    Vector2 basePosition_;
    Vector2 arrowOffset_;
    float columnSpacing_;
    float rowSpacing_;
    float yGroupOffsetY_;
    float buttonScale_;
    float arrowScale_;

public:
    /// Getter
    LayoutParam GetLayoutParam() const { return MakeLayoutParam(); }
    const Vector2& GetBasePosition() const { return basePosition_; }
    PlayerComboAttackData::TriggerCondition GetCurrentCondition() const { return currentCondition_; }

    void SetAttackController(PlayerComboAttackController* controller) { pAttackController_ = controller; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
};
