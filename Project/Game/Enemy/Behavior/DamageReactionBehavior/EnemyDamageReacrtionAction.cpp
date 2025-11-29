#include "EnemyDamageReacrtionAction.h"
/// obj
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Enemy/Types/BaseEnemy.h"
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Player/Player.h"
/// data
#include"Player/ComboCreator/PlayerComboAttackData.h"
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
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
    InitializeReaction();
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

void EnemyDamageReactionAction::InitializeReaction() {
    if (!pReactionData_ || !pPlayerCollisionInfo_) {
        return;
    }

    // リアクションパラメータの取得
    const auto& reactionParam = pReactionData_->GetReactionParam();

    // プレイヤー攻撃からY方向の力を取得
    blowYPower_ = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;

    // 状態の設定
    currentState_ = static_cast<ReactionState>(reactionParam.intReactionState);

    // 状態に応じた初期化
    switch (currentState_) {
    case ReactionState::Normal:
        totalReactionTime_ = 0.5f; // 通常のヒットストップ時間
        pBaseEnemy_->DamageRenditionInit();
        break;

    case ReactionState::Slammed:
        // バウンドパラメータの設定
        maxBoundCount_     = reactionParam.slammedParam.boundNum;
        bounceDamping_     = reactionParam.slammedParam.bounceDamping;
        currentBoundCount_ = 0;
        hasReachedGround_  = false;

        // blowYPowerを負の値として使用(地面に叩きつける)
        bounceSpeed_ = -std::abs(blowYPower_);

        totalReactionTime_ = 3.0f; // 全体の時間

        // 演出初期化
        pBaseEnemy_->DamageRenditionInit();
        break;

    case ReactionState::TakeUpper:
        // 打ち上げパラメータの設定
        floatingTime_   = reactionParam.takeUpperParam.floatingTime;
        hasReachedPeak_ = false;
        floatingTimer_  = 0.0f;

        // blowYPowerをそのまま使用(上に打ち上げ)
        jumpSpeed_ = std::abs(blowYPower_);

        initialPosition_   = pBaseEnemy_->GetWorldPosition();
        totalReactionTime_ = floatingTime_ + 2.0f; // 浮遊時間 + 落下時間

        // 演出初期化
        pBaseEnemy_->DamageRenditionInit();
        break;
    }

    reactionTimer_         = 0.0f;
    currentRenditionIndex_ = 0;
    hasPlayedRendition_    = false;
}

void EnemyDamageReactionAction::UpdateNormal() {
    // 通常のダメージリアクション
    // 短いヒットストップのみ
}

void EnemyDamageReactionAction::UpdateSlammed() {
    const auto& param = pBaseEnemy_->GetParameter();

    // まず地面に叩きつける
    if (!hasReachedGround_) {
        // 下向きの力で地面に叩きつける
        pBaseEnemy_->Fall(bounceSpeed_, 50.0f, 2.0f, false);

        // 地面に到達したか確認
        if (pBaseEnemy_->GetWorldPosition().y <= param.basePosY) {
            hasReachedGround_ = true;

            // 地面衝突エフェクト
            pBaseEnemy_->ThrustRenditionInit();

            // 最初のバウンド速度を設定(正の値で上向き)
            bounceSpeed_ = std::abs(blowYPower_) * 0.7f; // 初期の70%の力でバウンド
        }
        return;
    }

    // 地面到達後、バウンド処理
    if (currentBoundCount_ < maxBoundCount_) {
        // ジャンプとフォール
        pBaseEnemy_->Jump(bounceSpeed_, 30.0f, 1.5f);

        // 地面に着地したら次のバウンド
        if (pBaseEnemy_->GetWorldPosition().y <= param.basePosY) {
            currentBoundCount_++;

            // 速度を減衰
            bounceSpeed_ *= bounceDamping_;

            // バウンドエフェクト
            if (currentBoundCount_ < maxBoundCount_) {
                pBaseEnemy_->ThrustRenditionInit();
            }
        }
    } else {
        // 全てのバウンドが終わったら落下
        float fallSpeed = bounceSpeed_;
        pBaseEnemy_->Fall(fallSpeed, 30.0f, 1.0f, false);
    }
}

void EnemyDamageReactionAction::UpdateTakeUpper() {
    const auto& param = pBaseEnemy_->GetParameter();

    // 頂点に到達していない場合は上昇
    if (!hasReachedPeak_) {
        pBaseEnemy_->Jump(jumpSpeed_, 50.0f, 2.0f);

        // 速度が0以下になったら頂点到達
        if (jumpSpeed_ <= 0.0f) {
            hasReachedPeak_ = true;
            floatingTimer_  = 0.0f;
        }
    }
    // 頂点到達後、浮遊時間中
    else if (floatingTimer_ < floatingTime_) {
        // 浮遊状態を維持
        floatingTimer_ += Frame::DeltaTime();

        // 浮遊中はほぼ同じ高さを維持
        Vector3 currentPos = pBaseEnemy_->GetWorldPosition();
        float targetHeight = currentPos.y;

        // 微妙な浮遊表現(オプション)
        float floatOffset = std::sin(floatingTimer_ * 3.0f) * 0.2f;
        pBaseEnemy_->SetWorldPositionY(targetHeight + floatOffset);

        // 回転演出
        float rotateSpeed     = param.thrustRotateSpeed;
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + rotateSpeed * Frame::DeltaTime());
    }
    // 浮遊時間終了後、落下
    else {
        float fallSpeed = jumpSpeed_;
        pBaseEnemy_->Fall(fallSpeed, 50.0f, 2.0f, false);

        // 回転継続
        float rotateSpeed     = param.thrustRotateSpeed;
        float currentRotation = pBaseEnemy_->GetBodyRotation().x;
        pBaseEnemy_->SetBodyRotateX(currentRotation + rotateSpeed * Frame::DeltaTime());
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
    const auto& param = pBaseEnemy_->GetParameter();

    // タイマーが終了時間を超えたら終了
    if (reactionTimer_ >= totalReactionTime_) {
        return true;
    }

    // Slammed状態の場合、地面に到達してバウンドも終了したら終了
    if (currentState_ == ReactionState::Slammed) {
        if (hasReachedGround_ && currentBoundCount_ >= maxBoundCount_ && pBaseEnemy_->GetWorldPosition().y <= param.basePosY) {
            return true;
        }
    }

    // TakeUpper状態の場合、浮遊後に地面に着地したら終了
    if (currentState_ == ReactionState::TakeUpper) {
        if (hasReachedPeak_ && floatingTimer_ >= floatingTime_ && pBaseEnemy_->GetWorldPosition().y <= param.basePosY) {
            return true;
        }
    }

    return false;
}