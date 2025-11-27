#pragma once

#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "PlayerAttackRenditionData.h"
#include "utility/FileSelector/FileSelector.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

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
    };

    // コリジョンパラメータ
    struct CollisionParam {
        Vector3 size;
        Vector3 offsetPos;
        float adaptTime;
    };

    // タイミングパラメータ
    struct TimingParam {
        bool isCancel;
        float cancelTime;
        float precedeInputTime;
        float finishWaitTime;
    };

    // 攻撃発動に関するパラメータ
    struct TriggerParam {
        bool isFirstAttack;
        TriggerCondition condition;
        int32_t keyBordBottom;
        int32_t gamePadBottom;
    };

    struct LoopParam {
        int32_t num;
    };

    // アタックパラメータ
    struct AttackParameter {
        CollisionParam collisionPara;
        MoveParam moveParam;
        TimingParam timingParam;
        TriggerParam triggerParam;
        LoopParam loopParam;
        float knockBackPower;
        float power;
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

    bool IsReserveNextAttack(const float& currentTime, const TriggerParam& nextAtkTrigger);
    bool IsWaitFinish(const float& currentTime);
    bool IsAbleCancel(const float& currentTime, const TriggerParam& nextAtkTrigger);

private:
    //*-------------------------------- private Method --------------------------------*//

    // 次の攻撃の選択
    void SelectNextAttack();

private:
    //*-------------------------------- Private variants--------------------------------*//

    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";

    PlayerAttackRenditionData renditionData_;
    FileSelector fileSelector_;

    // 攻撃パラメータ
    AttackParameter attackParam_;
    bool needsRefresh_ = true;
    std::vector<std::string> attackFileNames_;

    int32_t tempCondition_;

public:
    //*-------------------------------- Getter Method --------------------------------*//
    const std::string& GetGroupName() const { return groupName_; }
    const AttackParameter& GetAttackParam() const { return attackParam_; }
    const PlayerAttackRenditionData& GetRenditionData() const { return renditionData_; }
};