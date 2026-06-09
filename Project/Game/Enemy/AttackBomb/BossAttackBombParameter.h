#pragma once
#include "AttackBomb.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <string>

/// <summary>
/// BossAttackBombManager 用パラメータ管理クラス
/// </summary>
class BossAttackBombParameter {
public:
    BossAttackBombParameter()  = default;
    ~BossAttackBombParameter() = default;

    void Init();
    void AdjustParam();

private:
    void RegisterParams();

    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "BossAttackBombManager";

    float bombThrowInterval_ = 3.0f;
    float stressPerBomb_     = 10.0f;
    AttackBomb::Param bombParam_;

public:
    const AttackBomb::Param& GetBombParam() const { return bombParam_; }
    float GetThrowInterval() const { return bombThrowInterval_; }
    float GetStressPerBomb() const { return stressPerBomb_; }
};
