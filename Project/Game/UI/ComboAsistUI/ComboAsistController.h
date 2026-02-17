#pragma once
// AsistParts
#include "AsistParts/ComboAsistArrowUI.h"
#include "AsistParts/ComboAsistButtonUI.h"
// Builder
#include "ComboPathBuilder.h"
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// Easing
#include "Easing/Easing.h"
// math
#include "Vector2.h"
// std
#include <cstdint>
#include <map>
#include <memory>
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
    // UI要素のグループ（パスデータはBuilderが保持）
    struct ComboUIGroup {
        std::vector<std::unique_ptr<ComboAsistButtonUI>> mainButtonUIs;
        std::vector<std::unique_ptr<ComboAsistArrowUI>> mainArrowUIs;

        std::vector<std::vector<std::unique_ptr<ComboAsistButtonUI>>> branchButtonUIs;
        std::vector<std::unique_ptr<ComboAsistArrowUI>> branchArrowUIs;
        std::vector<std::vector<std::unique_ptr<ComboAsistArrowUI>>> branchInnerArrowUIs;

        void Clear() {
            mainButtonUIs.clear();
            mainArrowUIs.clear();
            branchButtonUIs.clear();
            branchArrowUIs.clear();
            branchInnerArrowUIs.clear();
        }
    };

    // 発動条件ごとのUI・パスデータ
    struct ConditionUIData {
        ComboPathBuilder pathBuilder;
        ComboUIGroup xUIGroup;
        ComboUIGroup yUIGroup;
        int32_t columnShiftAmount = 0;
        bool isColumnShifting = false;

        void Clear() {
            xUIGroup.Clear();
            yUIGroup.Clear();
            columnShiftAmount = 0;
            isColumnShifting = false;
        }

        bool HasData() const {
            return !xUIGroup.mainButtonUIs.empty() || !yUIGroup.mainButtonUIs.empty();
        }
    };

public:
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

private:
    //=== UI生成 ===
    void BuildAllConditions();
    void CreateConditionUI(ConditionUIData& data);
    void CreateGroupUI(const ComboPathBuilder::ComboPathGroup& pathGroup, ComboUIGroup& uiGroup, int32_t* currentRow);

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

    //=== レイアウト情報の構築 ===
    LayoutParam MakeLayoutParam() const;

private:
    Player* pPlayer_                                = nullptr;
    PlayerComboAttackController* pAttackController_ = nullptr;
    KetaEngine::GlobalParameter* globalParameter_   = nullptr;
    std::string groupName_                          = "ComboAsistUI";

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

    float slideOffsetX_                           = 0.0f;
    bool isVisible_                               = false;
    bool isSliding_                               = false;

    // 表示範囲（minは常に0）
    int32_t maxVisibleColumn_ = 5;
    int32_t maxVisibleRow_    = 4;

    // 列オーバーフロー
    KetaEngine::Easing<float> columnOverflowEasing_;
    float overflowScale_      = 0.0f;

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
