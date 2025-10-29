#pragma once

#include "Combo/Combo.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <string>

class Combo;

/// <summary>
/// プレイヤー攻撃データクラス
/// </summary>
class PlayerComboAttackData {
public:
    struct AttackParameter {
        Vector3 collisionSize;
        float collisionOffsetValue;
        float adaptTime;
        float power;
    };

public:
    PlayerComboAttackData()  = default;
    ~PlayerComboAttackData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void Init(const std::string& attackName);

    void AdjustParam(); //< パラメータ調整
    void BindParams(); //< パラメータバインド
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    const std::string folderPath_ = "AttackCreator";

    // 攻撃パラメータ
    AttackParameter attackParam_;

public:
    std::string GetGroupName() const { return groupName_; }
    AttackParameter& GetAttackParam() { return attackParam_; }
};