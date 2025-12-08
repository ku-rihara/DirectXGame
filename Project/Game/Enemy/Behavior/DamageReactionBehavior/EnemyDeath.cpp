/// behavior
#include "EnemyDeath.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// math
#include "Frame/Frame.h"
#include "MathFunction.h"

// 初期化
EnemyDeath::EnemyDeath(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDeath", boss) {

    rotate_          = 0.0f;
    burstTime_       = 0.0f;
    blowJumpValue_   = pBaseEnemy_->GetParameter().deathBlowValueY;
    gravity_         = pBaseEnemy_->GetParameter().deathGravity;
    kFallSpeedLimit_ = 60.0f;
    step_            = Step::DIRECTIONSET;
}

void EnemyDeath::AngleCaluclation() {
    // プレイヤーの位置を取得
    Vector3 playerPos = pBaseEnemy_->GetPlayer()->GetWorldPosition();

    // プレイヤーから敵への方向ベクトルを計算
    direction_   = pBaseEnemy_->GetDirectionToTarget(playerPos);
    direction_.y = 0.0f;
    direction_   = direction_.Normalize();

    // プレイヤーを向く角度を計算
    objectiveAngle_ = std::atan2(direction_.x, direction_.z);
}

EnemyDeath::~EnemyDeath() {
}

void EnemyDeath::Update() {
    switch (step_) {
    /// ------------------------------------------------------
    /// 方向計算
    ///---------------------------------------------------------
    case Step::DIRECTIONSET:
        AngleCaluclation();
        blowPower_ = Vector3(
            direction_.x * -pBaseEnemy_->GetParameter().deathBlowValue * Frame::DeltaTimeRate(),
            0.0f,
            direction_.z * -pBaseEnemy_->GetParameter().deathBlowValue * Frame::DeltaTimeRate());
        step_ = Step::BLOW;
        break;

    /// -------------------------------------------------------
    /// 吹き飛び
    ///---------------------------------------------------------
    case EnemyDeath::Step::BLOW:
        pBaseEnemy_->SetRotationY(LerpShortAngle(
            pBaseEnemy_->GetBaseTransform().rotation_.y,
            objectiveAngle_,
            0.5f));
        burstTime_ += Frame::DeltaTimeRate();

        // 吹っ飛び回転
        rotate_ += pBaseEnemy_->GetParameter().deathRotateSpeed * Frame::DeltaTimeRate();
        pBaseEnemy_->SetRotationX(rotate_);

        // 吹っ飛び適応
        pBaseEnemy_->AddPosition(blowPower_);
        pBaseEnemy_->Jump(blowJumpValue_, kFallSpeedLimit_, gravity_);

        if (burstTime_ < pBaseEnemy_->GetParameter().deathBurstTime)
            break;
        step_ = Step::BURST;
        break;

    /// -------------------------------------------------------
    /// 爆散
    ///---------------------------------------------------------
    case EnemyDeath::Step::BURST:
        pBaseEnemy_->DeathRenditionInit();
        step_ = Step::DEATH;
        break;

    /// -------------------------------------------------------
    /// 死
    ///---------------------------------------------------------
    case EnemyDeath::Step::DEATH:
        pBaseEnemy_->SetIsDeath(true);
        break;

    default:
        break;
    }
}

void EnemyDeath::Debug() {
}