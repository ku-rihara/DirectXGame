#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"

//
#include "ComboBranchParameter.h"
#include "PlayerAttackRenditionData.h"
#include "Timeline/PlayerComboAttackTimeline.h"
#include "Timeline/PlayerComboAttackTimelinePhase.h"
// utility
#include "utility/FileSelector/FileSelector.h"
// math
#include "Vector3.h"
// std
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Player;
class PlayerComboAttackController;
class EnemyManager;

/// <summary>
/// プレイヤー攻撃データクラス
/// </summary>
class PlayerComboAttackData {
public:
    using TimelinePhase = AttackTimelinePhase;

    enum class TriggerCondition {
        GROUND, // 地上のみ
        AIR, // 空中のみ
        BOTH, // 両方
        DASH, // ダッシュ
        JUSTACTION // ジャスト回避とかのアクション
    };

    enum class CollisionFollowTarget {
        PLAYER,
        LEFTHAND,
        RIGHTHAND,
    };

public:
    // 移動パラメータ
    struct MoveParam {
        Vector3 value;
        int32_t easeType = 0;
        float easeTime;
        float startTime;
        bool isAbleInputMoving;
        bool isPositionYSelect;
        float finishTimeOffset;
        bool isStopBeforeEnemy = false;
    };

    // コリジョンパラメータ
    struct CollisionParam {
        CollisionFollowTarget followTarget = CollisionFollowTarget::PLAYER;
        float sphereRad;
        Vector3 offsetPos;
        float startTime;
        float adaptTime;
        float loopWaitTime;
        int32_t loopNum;
        bool isAlwaysFollowing;
    };

    // タイミングパラメータ
    struct TimingParam {
        float finishWaitTime;
        bool isAutoAdvance;
    };

    // 攻撃発動に関するパラメータ
    struct TriggerParam {
        TriggerCondition condition;
        int32_t keyBordBottom;
        int32_t gamePadBottom;
    };

    // アタックパラメータ
    struct AttackParameter {
        CollisionParam collisionParam;
        MoveParam moveParam;
        TimingParam timingParam;
        TriggerParam triggerParam;
        float knockBackPower;
        float power;
        float blowYPower;
        bool isMotionOnly = false;
        int32_t ableDefeatLevel = 0; 
        bool isUnlocked = true;      
    };

public:
    PlayerComboAttackData()  = default;
    ~PlayerComboAttackData() = default;

    //*-------------------------------- public Method --------------------------------*//

    // 初期化
    void Init(const std::string& attackName);

    // パラメータバインド、調節
    void AdjustParam();
    void RegisterParams();

    // データロード、セーブ
    void LoadData();
    void SaveData();

    bool IsReserveNextAttack(float currentTime, const ComboBranchParameter& branch, bool hasHitEnemy);
    bool IsWaitFinish(float currentTime);
    bool IsCancelAttack(float currentTime, const ComboBranchParameter& branch, bool hasHitEnemy);

    // コンボ分岐の初期化
    void InitComboBranches();
    // コンボ分岐のリサイズ
    void ResizeComboBranches();

    // フェーズ別パラメータ取得
    AttackParameter& GetAttackParamForPhase(TimelinePhase phase);
    const AttackParameter& GetAttackParamForPhase(TimelinePhase phase) const;
    PlayerAttackRenditionData& GetRenditionDataForPhase(TimelinePhase phase);
    const PlayerAttackRenditionData& GetRenditionDataForPhase(TimelinePhase phase) const;

    // コンボ分岐UI
    void DrawComboBranchesUI();
    void DrawCollisionParamUI();
    void DrawMoveParamUI();
    void DrawTriggerParamUI(bool isFirstAttack);
    void DrawFlagsParamUI();

    // フェーズ別UI
    void DrawCollisionParamUIForPhase(TimelinePhase phase);
    void DrawMoveParamUIForPhase(TimelinePhase phase);
    void DrawFlagsParamUIForPhase(TimelinePhase phase);
    void DrawPhaseControls();

    // 分岐トラックの再構築
    void RebuildBranchTracks();

    // セーブ、ロードのUI描画
    void DrawSaveLoadUI();

private:
    // TriggerConditionのチェック
    bool CheckTriggerCondition(TriggerCondition condition) const;

    // フェーズ別パラメータ登録
    void RegisterPhaseParams(const std::string& prefix, AttackParameter& param, PlayerAttackRenditionData& rendition);

private:
    //*-------------------------------- Private variants--------------------------------*//

    // global Parameter
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";

    Player* pPlayer_                          = nullptr;
    PlayerComboAttackController* pController_ = nullptr;
    EnemyManager* pEnemyManager_              = nullptr;

    // コンボ分岐リスト
    std::vector<std::unique_ptr<ComboBranchParameter>> comboBranches_;

    // メインフェーズ
    PlayerAttackRenditionData renditionData_;
    PlayerComboAttackTimeline timeLine_;
    AttackParameter attackParam_;

    // 予備動作フェーズ
    AttackParameter prepAttackParam_;
    PlayerAttackRenditionData prepRenditionData_;
    PlayerComboAttackTimeline prepTimeline_;

    // 終了処理フェーズ
    AttackParameter finishAttackParam_;
    PlayerAttackRenditionData finishRenditionData_;
    PlayerComboAttackTimeline finishTimeline_;

    // フェーズフラグ
    bool hasPrep_              = false;
    bool hasFinish_            = false;
    TimelinePhase currentDisplayPhase_ = TimelinePhase::MAIN;

    std::vector<std::string> attackFileNames_;

    // frags
    bool needsRefresh_ = true;
    bool useTimeline_  = true;

    // enum class Int
    int32_t collisionFollowTargetInt_;
    int32_t triggerConditionInt_;
    int32_t branchCount_ = 0;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const AttackParameter& GetAttackParam() const { return attackParam_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
    const PlayerAttackRenditionData& GetRenditionData() const { return renditionData_; }
    PlayerAttackRenditionData& GetRenditionData() { return renditionData_; }
    const std::string& GetFolderPath() const { return folderPath_; }
    Player* GetPlayer() const { return pPlayer_; };
    KetaEngine::TimelineDrawer* GetTimeline();

    bool HasPrepPhase() const { return hasPrep_; }
    bool HasFinishPhase() const { return hasFinish_; }
    TimelinePhase GetCurrentDisplayPhase() const { return currentDisplayPhase_; }
  
    const std::vector<std::unique_ptr<ComboBranchParameter>>& GetComboBranches() const { return comboBranches_; }
   

    void SetPlayer(Player* player);
    void SetController(PlayerComboAttackController* controller) { pController_ = controller; }
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }
    EnemyManager* GetEnemyManager() const { return pEnemyManager_; }
    void SetEffectEditorSuite(KetaEngine::EffectEditorSuite* suite);
};