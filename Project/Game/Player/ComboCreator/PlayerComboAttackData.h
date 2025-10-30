#pragma once

#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// プレイヤー攻撃データクラス
/// </summary>
class PlayerComboAttackData {
public:
    // 移動パラメータ
    struct MoveParam {
        Vector3 value;
        int32_t easeType = 0;
        Easing<Vector3> easing;
    };

    // コリジョンパラメータ
    struct CollisionParam {
        Vector3 size;
        Vector3 offsetPos;
        float adaptTime;
    };

    // タイミングパラメータ
    struct TimingParam {
        float cancelFrame;
        float precedeInputFrame;
    };

    // アタックパラメータ
    struct AttackParameter {
        CollisionParam collisionPara;
        MoveParam moveParam;
        TimingParam timingParam;
        float knockBackPower;
        float power;
        std::string nextAttackType;
    };

public:
    PlayerComboAttackData()  = default;
    ~PlayerComboAttackData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void Init(const std::string& attackName);

    /// <summary>
    /// イージングタイプセレクター
    /// </summary>
    /// <param name="label">ラベル</param>
    /// <param name="target">対象変数</param>
    void EasingTypeSelector(const char* label, int32_t& target);

     // パラメータバインド、調節
    void AdjustParam();
    void BindParams();

    // データロード、セーブ
    void LoadData();
    void SaveData();

private:

    // 次の攻撃の選択
    void SelectNextAttack(); 

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";


    // 攻撃パラメータ
    AttackParameter attackParam_;
    bool needsRefresh_ = true;
    std::vector<std::string> attackFileNames_;

public:
    const std::string& GetGroupName() const { return groupName_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
};