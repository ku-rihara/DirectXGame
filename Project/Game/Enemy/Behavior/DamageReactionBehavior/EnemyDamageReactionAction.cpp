#include "EnemyDamageReactionAction.h"
/// obj
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// data
#include "Player/ComboCreator/PlayerComboAttackData.h"
/// math
#include "EnemyDeath.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

// 初期化
EnemyDamageReactionAction::EnemyDamageReactionAction(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
    const PlayerCollisionInfo* playerCollisionInfo)
    : BaseEnemyDamageReaction("EnemyDamageReactionAction", boss) {

    pReactionData_        = reactionData;
    pPlayerCollisionInfo_ = playerCollisionInfo;

    // リアクションの初期化
    InitReaction();

    // 演出の初期化
    /*   damageRendition_ = std::make_unique<EnemyDamageRendition>();*/
    damageRendition_.Init(pBaseEnemy_, pReactionData_);
}

EnemyDamageReactionAction::~EnemyDamageReactionAction() {
}

void EnemyDamageReactionAction::Update(float deltaTime) {
    if (!pReactionData_) {
        return;
    }

     damageRendition_.Update(deltaTime, reactionTimer_, hasPlayedRendition_);

    // 状態に応じた更新処理
    switch (currentState_) {
    case ReactionState::Normal:
        UpdateNormal();
        break;
    case ReactionState::Slammed:
        UpdateNormal();
        UpdateSlammed();
        break;
    case ReactionState::TakeUpper:
        UpdateNormal();
        UpdateTakeUpper();
        break;
    }

 
    // タイマー更新
    reactionTimer_ += deltaTime;

    // リアクション終了チェック
    if (IsReactionFinished()) {
        // Rootに戻る前に処理を完了
        OnReactionEnd();
        // Rootに戻る
        pBaseEnemy_->BackToDamageRoot();
    }
}

void EnemyDamageReactionAction::Debug() {
}

void EnemyDamageReactionAction::InitReaction() {
    if (!pReactionData_ || !pPlayerCollisionInfo_) {
        return;
    }

    // リアクションパラメータの取得
    const auto& reactionParam = pReactionData_->GetReactionParam();

    // プレイヤー攻撃からパワーを取得
    blowYPower_     = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;
    knockBackPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().knockBackPower;

    // 状態の設定
    currentState_ = static_cast<ReactionState>(reactionParam.intReactionState);
    InitNormalReaction(reactionParam.normalParam);

    // 状態に応じた初期化
    switch (currentState_) {
    case ReactionState::Normal:
        break;

    case ReactionState::Slammed:
        InitSlammedReaction(reactionParam.boundParam, reactionParam.slammedParam);
        break;

    case ReactionState::TakeUpper:
        InitTakeUpperReaction(reactionParam.boundParam, reactionParam.takeUpperParam);
        break;
    }

    reactionTimer_         = 0.0f;
    currentRenditionIndex_ = 0;
    hasPlayedRendition_    = false;
}

void EnemyDamageReactionAction::InitNormalReaction(const EnemyDamageReactionData::NormalParam& param) {
    // プレイヤーから敵への方向ベクトルを計算
    Vector3 playerPos = pPlayerCollisionInfo_->GetPlayerTransform()->GetWorldPos();
    Vector3 enemyPos  = pBaseEnemy_->GetWorldPosition();
    Vector3 direction = enemyPos - playerPos;
    direction.y       = 0.0f; // Y方向は無視
    direction         = direction.Normalize();

    // ノックバック速度ベクトルを設定
    knockBackVelocity_ = direction * knockBackPower_;
    knockBackTimer_    = 0.0f;
    totalReactionTime_ = param.knockBackTime;

    // 演出初期化
    pBaseEnemy_->DamageRenditionInit();
}

void EnemyDamageReactionAction::InitSlammedReaction(
    const EnemyDamageReactionData::BoundParam& boundParam,
    const EnemyDamageReactionData::SlammedParam& param) {

    // バウンドパラメータの設定
    maxBoundCount_     = boundParam.boundNum;
    bounceDamping_     = boundParam.bounceDamping;
    initialBounceRate_ = boundParam.initialBounceRate;

    currentBoundCount_ = 0;
    hasReachedGround_  = false;

    // 下向きの速度を設定
    bounceSpeed_ = -std::abs(blowYPower_);

    // Slammed固有のパラメータ
    slammedGravity_     = param.gravity;
    slammedRotateSpeed_ = param.rotateSpeed;

    // 演出初期化
    pBaseEnemy_->DamageRenditionInit();
}

void EnemyDamageReactionAction::InitTakeUpperReaction(
    const EnemyDamageReactionData::BoundParam& boundParam,
    const EnemyDamageReactionData::TakeUpperParam& param) {

    // バウンドパラメータの設定
    maxBoundCount_     = boundParam.boundNum;
    bounceDamping_     = boundParam.bounceDamping;
    initialBounceRate_ = boundParam.initialBounceRate;

    currentBoundCount_ = 0;

    // 打ち上げパラメータの設定
    floatingTime_   = param.floatingTime;
    hasReachedPeak_ = false;
    floatingTimer_  = 0.0f;

    // 吹っ飛び速度
    jumpSpeed_ = std::abs(blowYPower_);

    initialPosition_ = pBaseEnemy_->GetWorldPosition();

    // TakeUpper固有のパラメータ
    takeUpperGravity_     = param.gravity;
    takeUpperRotateSpeed_ = param.rotateSpeed;
    takeUpperFallLimit_   = param.fallSpeedLimit;

    // 演出初期化
    pBaseEnemy_->DamageRenditionInit();
}

void EnemyDamageReactionAction::UpdateNormal() {
    if (!pReactionData_) {
        return;
    }

    const auto& param = pReactionData_->GetReactionParam().normalParam;

    // ノックバックタイマー更新
    knockBackTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // ノックバック時間内の場合
    if (knockBackTimer_ < param.knockBackTime) {
        // 減衰を適用
        float dampingFactor = 1.0f - (param.knockBackDamping * KetaEngine::Frame::DeltaTimeRate());
        knockBackVelocity_ *= dampingFactor;

        // 位置を更新
        Vector3 newPos = pBaseEnemy_->GetWorldPosition() + knockBackVelocity_ * KetaEngine::Frame::DeltaTimeRate();
        pBaseEnemy_->SetWorldPositionX(newPos.x);
        pBaseEnemy_->SetWorldPositionZ(newPos.z);
    }
}

void EnemyDamageReactionAction::UpdateSlammed() {
    if (!pReactionData_) {
        return;
    }

    const auto& enemyParam = pBaseEnemy_->GetParameter();

    // まず地面に叩きつける
    if (!hasReachedGround_) {
        // 重力を適用して加速
        bounceSpeed_ -= slammedGravity_ * KetaEngine::Frame::DeltaTimeRate();

        // 位置更新
        pBaseEnemy_->SetWorldPositionY(
            pBaseEnemy_->GetWorldPosition().y + bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate());

        // 地面に到達したか確認
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
            hasReachedGround_ = true;

            // 地面衝突エフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // HPが0以下の場合、1バウンド目で死亡処理
            if (pBaseEnemy_->GetIsDeathPending()) {
                pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_));
                return;
            }

            // 叩きつけられたスピードをベースに最初のバウンド速度を設定
            bounceSpeed_ = std::abs(bounceSpeed_) * initialBounceRate_;
        }
        return;
    }

    if (hasReachedGround_) {
        UpdateBounce(enemyParam.basePosY, slammedGravity_, slammedRotateSpeed_);
    }
}

void EnemyDamageReactionAction::UpdateTakeUpper() {
    if (!pReactionData_) {
        return;
    }

    const auto& enemyParam = pBaseEnemy_->GetParameter();

    // 頂点に到達していない場合は上昇
    if (!hasReachedPeak_) {
        // 重力を適用しながらジャンプ
        pBaseEnemy_->Jump(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_);

        // 回転演出
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + takeUpperRotateSpeed_ * KetaEngine::Frame::DeltaTimeRate());

        // 速度が0以下になったら頂点到達
        if (jumpSpeed_ <= 0.0f) {
            hasReachedPeak_ = true;
            floatingTimer_  = 0.0f;
        }
    }
    // 頂点到達後、浮遊時間中
    else if (floatingTimer_ < floatingTime_) {
        // 浮遊状態を維持
        floatingTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // 回転演出
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + takeUpperRotateSpeed_ * KetaEngine::Frame::DeltaTimeRate());
    }
    // 浮遊時間終了後、落下
    else {
        // 落下中
        pBaseEnemy_->Fall(jumpSpeed_, takeUpperFallLimit_, takeUpperGravity_, false);

        // 回転継続
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + takeUpperRotateSpeed_ * KetaEngine::Frame::DeltaTimeRate());

        // 地面に着地したらバウンド処理へ
        if (pBaseEnemy_->GetWorldPosition().y < enemyParam.basePosY) {
            pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);

            // 最初の着地時のバウンド速度を設定
            bounceSpeed_      = std::abs(blowYPower_) * initialBounceRate_;
            hasReachedGround_ = true;

            // バウンドエフェクト
            pBaseEnemy_->ThrustRenditionInit();
        }
    }

    // 地面到達後のバウンド処理
    if (hasReachedGround_) {
        UpdateBounce(enemyParam.basePosY, takeUpperGravity_, takeUpperRotateSpeed_);
    }
}

void EnemyDamageReactionAction::UpdateBounce(float basePosY, float gravity, float rotateSpeed) {
    if (currentBoundCount_ >= maxBoundCount_) {
        return;
    }

    // 重力を適用
    bounceSpeed_ -= gravity * KetaEngine::Frame::DeltaTimeRate();

    // 位置更新
    Vector3 currentPos = pBaseEnemy_->GetWorldPosition();
    currentPos.y += bounceSpeed_ * KetaEngine::Frame::DeltaTimeRate();
    pBaseEnemy_->SetWorldPositionY(currentPos.y);

    // 回転
    float currentRotation = pBaseEnemy_->GetBodyRotation().x;
    pBaseEnemy_->SetBodyRotateX(currentRotation + rotateSpeed * KetaEngine::Frame::DeltaTime());

    // 地面に着地したら次のバウンド
    if (currentPos.y <= basePosY) {
        pBaseEnemy_->SetWorldPositionY(basePosY);
        currentBoundCount_++;

        // 次のバウンド速度を計算
        float nextBounceSpeed = std::abs(bounceSpeed_) * bounceDamping_;

        if (currentBoundCount_ >= maxBoundCount_) {
            // バウンド終了
            bounceSpeed_ = 0.0f;
        } else {
            // 次のバウンド
            bounceSpeed_ = nextBounceSpeed;

            // バウンドエフェクト
            pBaseEnemy_->ThrustRenditionInit();
        }
    }
}

bool EnemyDamageReactionAction::IsReactionFinished() const {
    const auto& enemyParam = pBaseEnemy_->GetParameter();

    // Normal状態の場合、ノックバック時間が終了したら終了
    if (currentState_ == ReactionState::Normal) {
        return knockBackTimer_ >= totalReactionTime_;
    }

    // Slammed状態の場合、地面に到達してバウンドも終了したら終了
    if (currentState_ == ReactionState::Slammed) {
        if (hasReachedGround_ && currentBoundCount_ >= maxBoundCount_ && pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
            return true;
        }
    }

    if (currentState_ == ReactionState::TakeUpper) {
        if (hasReachedPeak_ && floatingTimer_ >= floatingTime_ && hasReachedGround_ && currentBoundCount_ >= maxBoundCount_ && pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY && std::abs(bounceSpeed_) < 0.01f) {
            return true;
        }
    }

    return false;
}

void EnemyDamageReactionAction::OnReactionEnd() {

    // 回転を0に戻す
    pBaseEnemy_->RotateInit();

    // 位置を正確に地面に設定
    const auto& enemyParam = pBaseEnemy_->GetParameter();
    pBaseEnemy_->SetWorldPositionY(enemyParam.basePosY);
}