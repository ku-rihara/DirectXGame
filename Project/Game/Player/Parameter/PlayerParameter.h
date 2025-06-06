#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "utility/ParameterEditor/GlobalParameter.h"

/// std
#include <array>
#include <string>


  struct UpperParm {
    float BackLashValue;
    float BackLashEaseTime;
};

struct JumpParm {
    float jumpSpeed;
    float gravity;
    float fallSpeedLimit;
};

struct ComboParm {
    float waitTime;
    float attackReach;
    float attackEaseMax;
};

/// Paramater構造体
struct Parameters {
    Vector3 startPos_;
    float rushDistance;
    float upperPosY;
    UpperParm upperParm;
    JumpParm normalJump;
    JumpParm bountJump;
    JumpParm upperJump;
    float moveSpeed;
    float fallSpeed;
    float attackRotate;
    float attackRotateAnit;
    float attackFloatValue;
    float rushEaseMax;
    float attackRotateEaseT;
    float attackFloatEaseT;
};

  enum  ComboNum {
    FIRST,
    SECOND,
    THIRD,
    FORTH,
};


class PlayerParameter {


private:
  
private:
    /// グローバルなパラメータ
    GlobalParameter* globalParameter_; /// グローバルパラメータ
    const std::string groupName_ = "Player"; /// グループ名

private:
    /// ===================================================
    /// private variaus
    /// ===================================================

    ///* コンボパラメータ
    std::array<ComboParm, 4> normalComboParms_;
    std::array<ComboParm, 2> jumpComboParms_;
    Parameters playerParams_;

public:
    PlayerParameter();

    /// ===================================================
    ///  public method
    /// ===================================================

    ///* 初期化
    void Init();
   
    /// ====================================================================
    /// Editor
    /// ====================================================================
    void ParamLoadForImGui();
    void AddParmGroup();
    void SetValues();
    void ApplyGlobalParameter();
    void AdjustParm();

    /// =========================================================================================
    /// getter
    /// =========================================================================================
    Parameters GetParamaters() const { return playerParams_; }
    ComboParm GetNormalComboParm(const ComboNum& index) const { return normalComboParms_[static_cast<int>(index)]; }
    ComboParm GetJumpComboParm(const ComboNum& index) const { return jumpComboParms_[static_cast<int>(index)]; }

    /// =========================================================================================
    /// setter
    /// =========================================================================================

};