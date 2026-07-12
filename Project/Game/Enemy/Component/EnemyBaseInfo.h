#pragma once

#include "Vector2.h"
#include "Vector3.h"

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class KillCounter;

/// <summary>
/// 敵が参照する外部クラスとパラメータをまとめた基本情報
/// </summary>
class EnemyBaseInfo {
public:
    enum class Type {
        NORMAL,
        STRONG,
        COUNT,
    };

    struct Parameter {
        Vector3 baseScale_;
        float collisionRad;
        Vector3 collisionOffset;
        Vector2 hpBarPosOffset;
        Vector2 hpGaugePosOffset;
        float hpBarWorldOffsetY;
        float groupIconWorldOffsetY;
        float basePosY;
        float burstTime;
        float hpMax = 115.0f;
        float chaseDistance;
        float chaseSpeed;
        float deathBlowValue;
        float deathBlowValueY;
        float deathGravity;
        float deathRotateSpeed;
        float deathBurstTime;
        float deathAnimTimeout; 
        float gaugeIncreaseValue;
        float waitCooldownTime;
    };

private:
    Player*       player_      = nullptr;
    GameCamera*   gameCamera_  = nullptr;
    EnemyManager* manager_     = nullptr;
    Combo*        combo_       = nullptr;
    KillCounter*  killCounter_ = nullptr;
    Type      type_{};
    Parameter parameter_{};

public:
    // getter
    Player*          GetPlayer()      const { return player_; }
    GameCamera*      GetGameCamera()  const { return gameCamera_; }
    EnemyManager*    GetManager()     const { return manager_; }
    Combo*           GetCombo()       const { return combo_; }
    KillCounter*     GetKillCounter() const { return killCounter_; }
    const Type&      GetType()        const { return type_; }
    const Parameter& GetParameter()   const { return parameter_; }

    // setter
    void SetPlayer(Player* player)            { player_      = player; }
    void SetGameCamera(GameCamera* camera)    { gameCamera_  = camera; }
    void SetManager(EnemyManager* manager)    { manager_     = manager; }
    void SetCombo(Combo* combo)               { combo_       = combo; }
    void SetKillCounter(KillCounter* counter) { killCounter_ = counter; }
    void SetParameter(Type type, const Parameter& param) {
        type_      = type;
        parameter_ = param;
    }
};
