#pragma once

// Bom
#include "AttackBomb.h"
#include "BossAttackBombParameter.h"
// Editor
#include "Editor/PostEffectEditor/PostEffectPlayer.h"
// Math
#include "Vector3.h"
// std
#include <memory>
#include <vector>

class GameCamera;
class StressGauge;
class Player;

/// <summary>
/// ボス専用攻撃ボム管理クラス
/// </summary>
class BossAttackBombManager {
public:
    BossAttackBombManager()  = default;
    ~BossAttackBombManager() = default;

    void Init();
    void Update();

    void Start();
    void Stop();

private:
    void ThrowBomb();

private:
    const BossAttackBombParameter* pParam_ = nullptr;

    // 他クラスのポインタ
    GameCamera* pGameCamera_ = nullptr;
    StressGauge* pStressGauge_ = nullptr;
    Player*     pPlayer_     = nullptr;
    KetaEngine::PostEffectPlayer postEffectPlayer_;

    // 攻撃ボムのコンテナ
    std::vector<std::unique_ptr<AttackBomb>> bombs_;

    // タイマーと状態
    float throwTimer_ = 0.0f;
    bool  isActive_   = false;

    // 位置
    Vector3 bossPos_;
    Vector3 playerPos_;

public:
    void SetBossPosition(const Vector3& pos)          { bossPos_     = pos; }
    void SetPlayerPosition(const Vector3& pos)        { playerPos_   = pos; }
    void SetGameCamera(GameCamera* camera)            { pGameCamera_ = camera; }
    void SetStressGauge(StressGauge* dt)                { pStressGauge_ = dt; }
    void SetPlayer(Player* player)                    { pPlayer_     = player; }
    void SetParam(const BossAttackBombParameter* p)   { pParam_      = p; }
};
