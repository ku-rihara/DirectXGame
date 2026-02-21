#pragma once

/// utility
#include "Editor/ParameterEditor/GlobalParameter.h"

/// std
#include <array>
#include <cstdint>
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
    Vector3 baseScale_;
    float rushDistance;
    float upperPosY;
    UpperParam upperParam;
    JumpParam normalJump;
    JumpParam boundJump;
    JumpParam upperJump;
    SpawnParam spawnParam;
    float moveSpeed;
    float dashSpeedMultiplier;
    float fallSpeed;
    float attackRotate;
    float attackRotateAnit;
    float attackFloatValue;
};


class PlayerParameter {
public:
    PlayerParameter()  = default;
    ~PlayerParameter() = default;

    /// ===================================================
    ///  public method
    /// ===================================================
   
    // 初期化
    void Init();

    void RegisterParams();//<パラメータバインド
    void AdjustParam();//<パラメータ編集

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Player";

private:
    /// ===================================================
    /// private variable
    /// ===================================================

    // コンボパラメータ
    Parameters playerParams_;

public:
    /// =========================================================================================
    /// getter
    /// =========================================================================================
    const Parameters& GetParameters() const { return playerParams_; }
  
    /// =========================================================================================
    /// setter
    /// =========================================================================================
};