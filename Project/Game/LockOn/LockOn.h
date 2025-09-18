#pragma once
#include "2d/Sprite.h"
#include "3d/ViewProjection.h"

#include "Vector3.h"

#include <list>
#include <memory>

class BaseEnemy;
class LockOn {
public:
    LockOn()  = default;
    ~LockOn() = default;

    void Init();
    void Update(const std::list<std::unique_ptr<BaseEnemy>>& enemies, const ViewProjection& viewProjection);
    void Draw();

    // 線形補間
    void LarpTimeIncrement(float incrementTime);

    bool IsTargetRange(const BaseEnemy& enemy, const ViewProjection& viewProjection, Vector3& positionView);
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
    float kDegreeToRadian_;
    float minDistance_;
    float maxDistance_;
    float angleRange_;

public:
    // Getter
    const BaseEnemy* GetEnemyTarget() { return target_; }
    Vector3 GetTargetPosition() const;
};
