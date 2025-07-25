#pragma once

#include "3d/ViewProjection.h"

/// utility
#include "utility/ParameterEditor/GlobalParameter.h"

/// std
#include <array>
#include <string>

struct UpperParam {
    float BackLashValue;
    float chargeTime;
};

struct JumpParam {
    float jumpSpeed;
    float gravity;
    float fallSpeedLimit;
};

struct ComboParam {
    float waitTime;
    float attackReach;
};

struct SpawnParam {
    float waitTime_;
};

/// Parameter構造体
struct Parameters {
    Vector3 startPos_;
    float rushDistance;
    float upperPosY;
    UpperParam upperParam;
    JumpParam normalJump;
    JumpParam boundJump;
    JumpParam upperJump;
    SpawnParam spawnParam;
    float moveSpeed;
    float fallSpeed;
    float attackRotate;
    float attackRotateAnit;
    float attackFloatValue;
};

enum ComboNum {
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
    /// private variable
    /// ===================================================

    ///* コンボパラメータ
    std::array<ComboParam, 4> normalComboParams_;
    std::array<ComboParam, 2> jumpComboParams_;
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
    void BindParams();
    void AdjustParam();

    /// =========================================================================================
    /// getter
    /// =========================================================================================
    Parameters GetParamaters() const { return playerParams_; }
    ComboParam GetNormalComboParm(const ComboNum& index) const { return normalComboParams_[static_cast<int>(index)]; }
    ComboParam GetJumpComboParm(const ComboNum& index) const { return jumpComboParams_[static_cast<int>(index)]; }

    /// =========================================================================================
    /// setter
    /// =========================================================================================
};