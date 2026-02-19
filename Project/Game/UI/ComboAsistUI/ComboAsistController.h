#pragma once
// UI生成・ヘルパー
#include "ComboUIBuilder.h"
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
/// UI生成はComboUIBuilderに委譲し、表示制御・入力・演出を担当する
/// </summary>
class ComboAsistController {
public:
    enum class PanelMode { Close, Open };

    ComboAsistController()  = default;
    ~ComboAsistController() = default;

    void Init();
    void Update();
    void AdjustParam();

    //=== スライドイン・アウト ===
    void StartSlideIn();
    void StartSlideOut();
    void UpdateSlide(float deltaTime);
    void ApplySlideOffset();

    //=== パネル開閉 ===
    void OpenPanel();
    void TogglePanelMode();

private:
    //=== UI生成 ===
    void RebuildAllConditions();

    //=== ロック同期 ===
    void SyncUnlockStates();
    void SyncGroupUnlockStates(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup);

    //=== 攻撃発動演出 ===
    void UpdateComboState();
    void SetGroupActiveOutLines(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup, const std::unordered_set<std::string>& activeAttacks);
    void PlayPushScalingForAttack(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup, const std::string& attackName);

    //=== トグル入力 ===
    void CheckToggleInput();

    //=== 発動条件切替 ===
    void CheckConditionSwitchInput();
    void CheckAutoConditionSwitch();
    void SwitchCondition(PlayerComboAttackData::TriggerCondition condition);
    void SetConditionVisible(ConditionUIData& data, bool visible);

    //=== 表示範囲・列オーバーフロー ===
    void CheckColumnOverflow();
    void ShiftAllColumns(int32_t delta);
    void UpdateVisibility();

    //=== パラメータ ===
    void RegisterParams();

    //=== ラムダ置き換え用 ===
    void UpdateCurrentConditionUI();
    void ApplySlideOffsetToCondition(ConditionUIData& data);
    void SnapConditionToTarget(ConditionUIData& data);
    bool CheckGroupColumnOverflow(ConditionUIData& data, ComboUIGroup& uiGroup);
    void ShiftGroup(ComboUIGroup& uiGroup, int32_t delta);
    bool IsInVisibleRange(int32_t col, int32_t row) const;
    bool IsArrowVisible(const ComboAsistArrowUI& arrow) const;
    void UpdateGroupVisibility(ComboUIGroup& uiGroup);

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
    bool isVisible_           = false;
    bool isSliding_           = false;

    // パネル開閉モード
    PanelMode panelMode_      = PanelMode::Close;
    bool isModeTransitioning_ = false;

    // 表示範囲（minは常に0）
    int32_t maxVisibleColumn_ = 5;
    int32_t maxVisibleRow_    = 4;

    // 列オーバーフロー
    KetaEngine::Easing<float> columnOverflowEasing_;
    float overflowScale_ = 0.0f;

    // 発動条件切替
    PlayerComboAttackData::TriggerCondition currentCondition_ = PlayerComboAttackData::TriggerCondition::GROUND;
    bool isAutoSwitchedCondition_ = false; // 自動切替されたかどうか

    // レイアウトパラメータ
    Vector2 basePosition_;
    Vector2 arrowOffset_; // ボタン中心からの矢印オフセット
    float columnSpacing_; // ボタン間の横間隔
    float rowSpacing_;    // パス間の縦間隔
    float branchYOffset_; // 分岐ボタンのY座標オフセット
    float buttonScale_;   // ボタンUIのスケール
    float arrowScale_;    // 矢印UIのスケール

public:
    void SetAttackController(PlayerComboAttackController* controller) { pAttackController_ = controller; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
};
