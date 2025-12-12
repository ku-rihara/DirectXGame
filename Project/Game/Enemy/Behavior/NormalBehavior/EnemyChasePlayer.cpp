/// behavior
#include "EnemyChasePlayer.h"
#include "EnemyWait.h"

/// math
#include "MathFunction.h"
/// obj
#include "Enemy/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h" 
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>
/// std
#include <cmath>

// 初期化
EnemyChasePlayer::EnemyChasePlayer(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemyChasePlayer", boss) {

    pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
    pBaseEnemy_->GetFindSprite()->SetScale(Vector2(0, 0));

    spriteEasing_.Init("EnemyFindSpriteScaling.json");
    spriteEasing_.SetAdaptValue(&tempSpriteScale_);
    spriteEasing_.Reset();

    scaleEasing_.Init("EnemyFindScaling.json");
    scaleEasing_.SetAdaptValue(&tempEnemyScale_);
    scaleEasing_.Reset();

    isChase_ = true; // デバッグ用

    // 追従時間管理の初期化
    currentChaseTime_ = 0.0f;
    chaseResetTimer_  = 0.0f;
    isChaseTimeOver_  = false;
}

EnemyChasePlayer::~EnemyChasePlayer() {
}

void EnemyChasePlayer::Update() {

    spriteEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
    pBaseEnemy_->GetFindSprite()->SetScale(tempSpriteScale_);

    // Enemy AmplitudeScaling
    scaleEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
    pBaseEnemy_->SetScale(tempEnemyScale_);

    // ターゲットへのベクトル
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

    // 距離計算
    distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

    // 追従時間超過時のリセット処理
    if (isChaseTimeOver_) {
        chaseResetTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // リセット時間が経過したら追従を再開
        if (chaseResetTimer_ >= pBaseEnemy_->GetParameter().chaseResetTime) {
            currentChaseTime_ = 0.0f;
            chaseResetTimer_  = 0.0f;
            isChaseTimeOver_  = false;
        } else {
            // リセット中は待機状態に移行
            pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
            return;
        }
    }

    // 追従時間の更新
    UpdateChaseTime(KetaEngine::Frame::DeltaTimeRate());

    // 追従時間が最大値を超えた場合
    if (currentChaseTime_ >= pBaseEnemy_->GetParameter().maxChaseTime) {
        isChaseTimeOver_ = true;
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
        return;
    }

    // 一定距離で見失う
    if (distance_ > pBaseEnemy_->GetParameter().chaseDistance) {
        // 追従時間をリセット
        currentChaseTime_ = 0.0f;
        isChaseTimeOver_  = false;
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
        return;
    }

     // 近すぎる場合の早期リターン
    if (distance_ <= pBaseEnemy_->GetParameter().chaseLimitDistance) {
        return;
    }


    // 正規化
    direction.y = 0.0f;
    direction.Normalize();

    // 他の敵との回避ベクトルを計算
    Vector3 avoidanceVector = CalculateAvoidanceVector();

    // 最終的な移動方向を計算
    Vector3 finalDirection = direction + avoidanceVector;

    // 正規化
    if (finalDirection.Length() > 0.001f) {
        finalDirection.Normalize();
    } else {
    
        return;
    }

    /// 変位加算
    pBaseEnemy_->AddPosition(finalDirection * (pBaseEnemy_->GetParameter().chaseSpeed * KetaEngine::Frame::DeltaTime()));

    // 目標角度を計算
    float objectiveAngle = std::atan2(-finalDirection.x, -finalDirection.z);

    // 最短角度補間でプレイヤーの回転を更新
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
}

Vector3 EnemyChasePlayer::CalculateAvoidanceVector() {
    Vector3 avoidanceVector = Vector3::ZeroVector();

    // EnemyManagerから全ての敵を取得
    const auto& enemies = pBaseEnemy_->GetManager()->GetEnemies();

    Vector3 myPosition    = pBaseEnemy_->GetWorldPosition();
    float avoidanceRadius = pBaseEnemy_->GetParameter().avoidanceRadius;

    for (const auto& enemy : enemies) {
        // 自分自身はスキップ
        if (enemy.get() == pBaseEnemy_) {
            continue;
        }

        // 死亡している敵はスキップ
        if (enemy->GetIsDeath()) {
            continue;
        }

        Vector3 otherPosition = enemy->GetWorldPosition();
        Vector3 toOther       = otherPosition - myPosition;
        toOther.y             = 0.0f; // Y軸は無視

        float distance = toOther.Length();

        // 回避範囲内にいる場合
        if (distance < avoidanceRadius && distance > 0.001f) {
  
            float avoidanceStrength = (avoidanceRadius - distance) / avoidanceRadius;
            Vector3 avoidDirection  = -toOther.Normalize();
            avoidanceVector += avoidDirection * avoidanceStrength;
        }
    }

    // 回避ベクトルを正規化して適切な強度にする
    if (avoidanceVector.Length() > 0.001f) {
        avoidanceVector = avoidanceVector.Normalize() * 0.8f; 
    }

    return avoidanceVector;
}

void EnemyChasePlayer::UpdateChaseTime(float deltaTime) {
    // 追従中の時間を加算
    if (!isChaseTimeOver_) {
        currentChaseTime_ += deltaTime;
    }
}

void EnemyChasePlayer::Debug() {
#ifdef _DEBUG

#endif
}