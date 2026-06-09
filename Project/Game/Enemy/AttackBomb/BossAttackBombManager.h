#pragma once

// Bom
#include "AttackBomb.h"
// Editor
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Editor/PostEffectEditor/PostEffectPlayer.h"
// Math
#include "Vector3.h"
// std
#include <memory>
#include <string>
#include <vector>

class GameCamera;
class DeathTimer;
class Player;
/// <summary>
/// ボス専用攻撃ボム管理クラス
/// </summary>
class BossAttackBombManager {
public:
    BossAttackBombManager()  = default;
    ~BossAttackBombManager() = default;

    // 初期化と更新
    void Init();
    void Update();

    // 攻撃開始と終了
    void Start();
    void Stop();

    // パラメータ調整
    void AdjustParam();

private:
    void ThrowBomb();
    void RegisterParams();

private:

    // global parameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "BossAttackBombManager";

    // 他クラスのポインタ
    GameCamera*  pGameCamera_  = nullptr;
    DeathTimer*  pDeathTimer_  = nullptr;
    Player*      pPlayer_      = nullptr;
    KetaEngine::PostEffectPlayer postEffectPlayer_;

    // 攻撃ボムのコンテナ
    std::vector<std::unique_ptr<AttackBomb>> bombs_;

    // ボムのパラメータ
    float bombThrowInterval_ = 3.0f;
    float stressPerBomb_     = 10.0f;
    float arcHeight_         = 8.0f;
    float flightTime_        = 1.5f;

    // タイマーと状態
    float throwTimer_ = 0.0f;
    bool isActive_    = false;

    // 位置
    Vector3 bossPos_;
    Vector3 playerPos_;

public:
    void SetBossPosition(const Vector3& pos) { bossPos_ = pos; }
    void SetPlayerPosition(const Vector3& pos) { playerPos_ = pos; }
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }
    void SetDeathTimer(DeathTimer* dt) { pDeathTimer_ = dt; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
};
