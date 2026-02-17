#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"

//
#include "ComboBranchParameter.h"
#include "PlayerAttackRenditionData.h"
#include "Timeline/PlayerComboAttackTimeline.h"
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
    enum class TriggerCondition {
        GROUND, // 地上のみ
        AIR, // 空中のみ
        BOTH, // 両方
        DASH, // ダッシュ
        JUSTACTION // ジャスト回避とかのアクション
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
    };

    // コリジョンパラメータ
    struct CollisionParam {
        Vector3 size;
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

    // 落下パラメータ
    struct FallParam {
        bool enableFall;
    };

    // アタックパラメータ
    struct AttackParameter {
        CollisionParam collisionParam;
        MoveParam moveParam;
        TimingParam timingParam;
        TriggerParam triggerParam;
        FallParam fallParam;
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

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackName">攻撃名</param>
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

    // コンボ分岐UI
    void DrawComboBranchesUI();
    void DrawCollisionParamUI();
    void DrawMoveParamUI();
    void DrawTriggerParamUI(bool isFirstAttack);
    void DrawFlagsParamUI();

    // 分岐トラックの再構築
    void RebuildBranchTracks();

    // セーブ、ロードのUI描画
    void DrawSaveLoadUI();

private:
    // TriggerConditionのチェック
    bool CheckTriggerCondition(TriggerCondition condition) const;

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

    // 包含
    PlayerAttackRenditionData renditionData_;
    PlayerComboAttackTimeline timeLine_;

    // 攻撃パラメータ
    AttackParameter attackParam_;
    std::vector<std::string> attackFileNames_;

    // frags
    bool needsRefresh_ = true;
    bool useTimeline_  = true;

    // enum class Int
    int32_t triggerConditionInt_;
    int32_t branchCount_ = 0;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const AttackParameter& GetAttackParam() const { return attackParam_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
    const PlayerAttackRenditionData& GetRenditionData() const { return renditionData_; }
    KetaEngine::GlobalParameter* GetGlobalParameter() const { return globalParameter_; }
    const std::string& GetFolderPath() const { return folderPath_; }
    Player* GetPlayer() const { return pPlayer_; };
    KetaEngine::TimelineDrawer* GetTimeline();
  
    const std::vector<std::unique_ptr<ComboBranchParameter>>& GetComboBranches() const { return comboBranches_; }
   

    void SetPlayer(Player* player);
    void SetController(PlayerComboAttackController* controller) { pController_ = controller; }
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }
};