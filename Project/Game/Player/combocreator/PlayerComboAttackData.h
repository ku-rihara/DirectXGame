#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"

//
#include "PlayerAttackRenditionData.h"
#include "Timeline/PlayerComboAttackTimeline.h"
// utility
#include "utility/FileSelector/FileSelector.h"
// math
#include "Vector3.h"
// std
#include <cstdint>
#include <string>
#include <vector>

class Player;

/// <summary>
/// プレイヤー攻撃データクラス
/// </summary>
class PlayerComboAttackData {
public:
    enum class TriggerCondition {
        GROUND, // 地上のみ
        AIR, // 空中のみ
        BOTH // 両方
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
        bool isCancel;
        float cancelTime;
        float precedeInputTime;
        float finishWaitTime;
        bool isAutoAdvance;
    };

    // 攻撃発動に関するパラメータ
    struct TriggerParam {
        bool isFirstAttack;
        TriggerCondition condition;
        int32_t keyBordBottom;
        int32_t gamePadBottom;
        bool requireHit;
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
        std::string nextAttackType;
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

    bool IsReserveNextAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy);
    bool IsWaitFinish(float currentTime);
    bool IsCancelAttack(float currentTime, const TriggerParam& nextAtkTrigger, bool hasHitEnemy);

    // 次の攻撃の選択
    void SelectNextAttack();
    void DrawCollisionParamUI();
    void DrawMoveParamUI();
    void DrawTriggerParamUI();
    void DrawFlagsParamUI();

    // セーブ、ロードのUI描画
    void DrawSaveLoadUI();

private:
    //*-------------------------------- private Method --------------------------------*//

private:
    //*-------------------------------- Private variants--------------------------------*//

    // global Parameter
    KetaEngine::GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";

    Player* pPlayer_ = nullptr;

    // 包含
    PlayerAttackRenditionData renditionData_;
    PlayerComboAttackTimeline timeLine_;

    // file Selector
    KetaEngine::FileSelector fileSelector_;

    // 攻撃パラメータ
    AttackParameter attackParam_;
    std::vector<std::string> attackFileNames_;

    // frags
    bool needsRefresh_ = true;
    bool useTimeline_  = true;

    // enum class Int
    int32_t triggerConditionInt_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const AttackParameter& GetAttackParam() const { return attackParam_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
    const PlayerAttackRenditionData& GetRenditionData() const { return renditionData_; }
    KetaEngine::GlobalParameter* GetGlobalParameter() const { return globalParameter_; }
    const std::string& GetFolderPath() const { return folderPath_; }
    Player* GetPlayer() const { return pPlayer_; };

    void SetPlayer(Player* player);

};