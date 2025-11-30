#include "EnemyDamageReacrtionAction.h"
/// obj
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// data
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
/// math
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
}

EnemyDamageReactionAction::~EnemyDamageReactionAction() {
}

void EnemyDamageReactionAction::Update() {
    if (!pReactionData_) {
        return;
    }

    // 状態に応じた更新処理
    switch (currentState_) {
    case ReactionState::Normal:
        UpdateNormal();
        break;
    case ReactionState::Slammed:
        UpdateSlammed();
        break;
    case ReactionState::TakeUpper:
        UpdateTakeUpper();
        break;
    }

    // 演出の更新
    UpdateRenditions();

    // タイマー更新
    reactionTimer_ += Frame::DeltaTime();

    // リアクション終了チェック
    if (IsReactionFinished()) {
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

    // 状態に応じた初期化
    switch (currentState_) {
    case ReactionState::Normal:
        InitNormalReaction(reactionParam.normalParam);
        break;

    case ReactionState::Slammed:
        InitSlammedReaction(reactionParam.slammedParam);
        break;

    case ReactionState::TakeUpper:
        InitTakeUpperReaction(reactionParam.takeUpperParam);
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

void EnemyDamageReactionAction::InitSlammedReaction(const EnemyDamageReactionData::SlammedParam& param) {
    // バウンドパラメータの設定
    maxBoundCount_     = param.boundNum;
    bounceDamping_     = param.bounceDamping;
    currentBoundCount_ = 0;
    hasReachedGround_  = false;

    // 下向きの速度を設定
    bounceSpeed_ = -param.downSpeed;

    // 演出初期化
    pBaseEnemy_->DamageRenditionInit();
}

void EnemyDamageReactionAction::InitTakeUpperReaction(const EnemyDamageReactionData::TakeUpperParam& param) {
    // 打ち上げパラメータの設定
    floatingTime_   = param.floatingTime;
    hasReachedPeak_ = false;
    floatingTimer_  = 0.0f;

    // ジャンプ速度を設定
    jumpSpeed_ = std::abs(blowYPower_) * param.jumpSpeedRate;

    initialPosition_ = pBaseEnemy_->GetWorldPosition();

    // 演出初期化
    pBaseEnemy_->DamageRenditionInit();
}

void EnemyDamageReactionAction::UpdateNormal() {
    if (!pReactionData_) {
        return;
    }

    const auto& param = pReactionData_->GetReactionParam().normalParam;

    // ノックバックタイマー更新
    knockBackTimer_ += Frame::DeltaTime();

    // ノックバック時間内の場合
    if (knockBackTimer_ < param.knockBackTime) {
        // 減衰を適用
        float dampingFactor = 1.0f - (param.knockBackDamping * Frame::DeltaTimeRate());
        knockBackVelocity_ *= dampingFactor;

        // 位置を更新
        Vector3 newPos = pBaseEnemy_->GetWorldPosition() + knockBackVelocity_ * Frame::DeltaTimeRate();
        pBaseEnemy_->SetWorldPosition(newPos);
    }
}

void EnemyDamageReactionAction::UpdateSlammed() {
    if (!pReactionData_) {
        return;
    }

    const auto& reactionParam = pReactionData_->GetReactionParam().slammedParam;
    const auto& enemyParam    = pBaseEnemy_->GetParameter();

    // まず地面に叩きつける
    if (!hasReachedGround_) {
        // 下向きの力で地面に叩きつける
        pBaseEnemy_->Fall(bounceSpeed_, reactionParam.fallSpeedLimit, reactionParam.gravity, false);

        // 地面に到達したか確認
        if (pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
            hasReachedGround_ = true;

            // 地面衝突エフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // 最初のバウンド速度を設定
            bounceSpeed_ = std::abs(blowYPower_) * reactionParam.initialBounceRate;
        }
        return;
    }

    // 地面到達後、バウンド処理
    if (currentBoundCount_ < maxBoundCount_) {
        // ジャンプとフォール
        pBaseEnemy_->Jump(bounceSpeed_, reactionParam.fallSpeedLimit, reactionParam.gravity);

        // 地面に着地したら次のバウンド
        if (pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
            currentBoundCount_++;

            // 速度を減衰
            bounceSpeed_ *= bounceDamping_;

            // バウンドエフェクト
            if (currentBoundCount_ < maxBoundCount_) {
                pBaseEnemy_->ThrustRenditionInit();
            }

            // 回転
            float currentRotation = pBaseEnemy_->GetBodyRotation().x;
            pBaseEnemy_->SetBodyRotateX(currentRotation + reactionParam.thrustRotateSpeed * Frame::DeltaTime());
        }
    } else {
        // 全てのバウンドが終わったら落下
        float fallSpeed = bounceSpeed_;
        pBaseEnemy_->Fall(fallSpeed, reactionParam.fallSpeedLimit, reactionParam.gravity, false);
    }
}

void EnemyDamageReactionAction::UpdateTakeUpper() {
    if (!pReactionData_) {
        return;
    }

    const auto& reactionParam = pReactionData_->GetReactionParam().takeUpperParam;
    const auto& enemyParam    = pBaseEnemy_->GetParameter();

    // 頂点に到達していない場合は上昇
    if (!hasReachedPeak_) {
        pBaseEnemy_->Jump(jumpSpeed_, reactionParam.fallSpeedLimit, reactionParam.gravity);

        // 速度が0以下になったら頂点到達
        if (jumpSpeed_ <= 0.0f) {
            hasReachedPeak_ = true;
            floatingTimer_  = 0.0f;
        }
    }
    // 頂点到達後、浮遊時間中
    else if (floatingTimer_ < floatingTime_) {
        // 浮遊状態を維持（その場に停止）
        floatingTimer_ += Frame::DeltaTime();

        // 回転演出
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + reactionParam.thrustRotateSpeed * Frame::DeltaTime());
    }
    // 浮遊時間終了後、落下
    else {
        float fallSpeed = jumpSpeed_;
        pBaseEnemy_->Fall(fallSpeed, reactionParam.fallSpeedLimit, reactionParam.gravity, false);

        // 回転継続
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + reactionParam.thrustRotateSpeed * Frame::DeltaTime());
    }
}

void EnemyDamageReactionAction::UpdateRenditions() {
    if (!pReactionData_) {
        return;
    }

    // 演出データの取得
    const auto& renditions = pReactionData_->GetAllRenditions();

    if (currentRenditionIndex_ >= static_cast<int32_t>(renditions.size())) {
        return;
    }

    // 現在の演出データ
    const auto* currentRendition = renditions[currentRenditionIndex_].get();
    if (!currentRendition) {
        return;
    }

    // 演出のタイミングチェック
    const auto& animParam = currentRendition->GetObjAnimationParam();

    if (!hasPlayedRendition_ && reactionTimer_ >= animParam.startTiming) {
        // アニメーション再生処理(実装に応じて適切なメソッドを呼び出す)
        // pBaseEnemy_->PlayAnimation(animParam.fileName, animParam);

        hasPlayedRendition_ = true;
        currentRenditionIndex_++;

        // 次の演出に移行
        if (currentRenditionIndex_ < static_cast<int32_t>(renditions.size())) {
            hasPlayedRendition_ = false;
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

    // TakeUpper状態の場合、浮遊後に地面に着地したら終了
    if (currentState_ == ReactionState::TakeUpper) {
        if (hasReachedPeak_ && floatingTimer_ >= floatingTime_ && pBaseEnemy_->GetWorldPosition().y <= enemyParam.basePosY) {
            return true;
        }
    }

    return false;
}