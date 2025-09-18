#pragma once
#include "2d/Sprite.h"
#include "3d/ViewProjection.h"

#include "Vector3.h"

#include <list>
#include <memory>
#include <variant>

class BaseEnemy;
class Player;
class LockOn {
public:
    using LockOnVariant = std::variant<BaseEnemy*>;

public:
    LockOn()  = default;
    ~LockOn() = default;

    void Init();
    void Update(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
    void Draw();

    // 線形補間
    void LerpTimeIncrement(float incrementTime);

    bool IsTargetRange(const LockOnVariant& target, const Player* player, Vector3& relativePosition)const;
    bool IsOutOfRange(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
    void Search(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);

    bool ExistTarget() const { return target_ ? true : false; }
    void OnEnemyDestroyed(BaseEnemy* enemy);

private:
    std::unique_ptr<Sprite> lockOnMark_; //<スプライト
    const BaseEnemy* target_ = nullptr; //<ターゲット

    float lerpTime_; //< 線形補間タイム
    Vector2 prePos_; //< ロックオン前の座標

    Vector2 lockOnMarkPos_; //< スプライト座標
    float spriteRotation_; //< スプライトz回転

    // parameter
    float minDistance_; //< 最小距離
    float maxDistance_; //< 最大距離
    float angleRange_;  //< 角度範囲（度）

public:
    // Getter
    const BaseEnemy* GetEnemyTarget() { return target_; }
    Vector3 GetTargetPosition() const;
};
