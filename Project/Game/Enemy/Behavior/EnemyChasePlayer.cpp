
/// behavior
#include "EnemyChasePlayer.h"
#include "EnemyWait.h"

/// math
#include "MathFunction.h"
/// obj
#include "Enemy/BaseEnemy.h"
#include "Player/Player.h" /// プレイヤーの座標取得に使ってるので消さないこと
/// frame
#include "Frame/Frame.h"
/// imgui
#include <imgui.h>
/// std
#include <cmath>

// 初期化
EnemyChasePlayer::EnemyChasePlayer(BaseEnemy* boss)
    : BaseEnemyMoveBehavior("EnemyChasePlayer", boss) {

    // パラメータ初期化
    waveAttackStartPos_   = 25.0f; /// ボス
    normalAttackStartPos_ = 7.0f;
    attackCoolTime_       = 1.0f;

    pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
    pBaseEnemy_->GetFindSprite()->SetScale(Vector2(0, 0));
    spriteEase_.maxTime = 0.5f;
    spriteEase_.time    = 0.0f;

    scaleEase_.time      = 0.0f;
    scaleEase_.maxTime   = 0.6f;
    scaleEase_.amplitude = 1.5f;
    scaleEase_.period    = 0.2f;

    isChase_ = true; //	デバッグ用
}

EnemyChasePlayer::~EnemyChasePlayer() {
}

void EnemyChasePlayer::Update() {

    // Sherching Sprite Easing
    spriteEase_.time += Frame::DeltaTime();
    spriteEase_.time = std::min(spriteEase_.time, spriteEase_.maxTime);
    pBaseEnemy_->GetFindSprite()->SetScale(EaseOutBack(Vector2::ZeroVector(), Vector2::UnitVector(), spriteEase_.time, spriteEase_.maxTime));

    // Enemy AmplitudeScaling
    scaleEase_.time += Frame::DeltaTime();
    scaleEase_.time = std::min(scaleEase_.time, scaleEase_.maxTime);
    pBaseEnemy_->SetScale(EaseAmplitudeScale(pBaseEnemy_->GetParameter().initScale_, scaleEase_.time, scaleEase_.maxTime, scaleEase_.amplitude, scaleEase_.period));

    // ターゲットへのベクトル
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

    // 距離計算
    distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

    // 　一定距離で見失う
    if (distance_ > pBaseEnemy_->GetParameter().chaseDistance) {
        pBaseEnemy_->ChangeMoveBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
        return;
    }

    // 　近すぎる場合の早期リターン
    if (distance_ <= 7.0f) {
        return;
    }

    // 正規化
    direction.y = 0.0f;
    direction.Normalize();

    /// 変位加算
    pBaseEnemy_->AddPosition(direction * (pBaseEnemy_->GetParameter().chaseSpeed * Frame::DeltaTime()));

    // 目標角度を計算
    float objectiveAngle = std::atan2(-direction.x, -direction.z);

    // 最短角度補間でプレイヤーの回転を更新
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle, 0.3f));
}

void EnemyChasePlayer::Debug() {
}
