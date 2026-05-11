#include "StrongEnemyFleeBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
#include "Field/Field.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include "Player/Player.h"
#include <cmath>

StrongEnemyFleeBehavior::StrongEnemyFleeBehavior(StrongEnemy* enemy, float startDelay)
    : StrongEnemyBehaviorBase("StrongEnemyFleeBehavior", enemy), startDelayTimer_(startDelay) {

    if (startDelayTimer_ <= 0.0f) {
        pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
    }

    // 焦りエフェクトの発生
    pBaseEnemy_->GetEnemyEffects()->Emit("EnemyImpatience");
}

void StrongEnemyFleeBehavior::Update() {
    const float dt    = KetaEngine::Frame::DeltaTime();
    const auto& param = GetEnemy()->GetStrongParameter();

    // ── クールタイム中 ──────────────────────────────────
    if (startDelayTimer_ > 0.0f) {
        startDelayTimer_ -= dt;
        pBaseEnemy_->DirectionToPlayer();
        if (startDelayTimer_ <= 0.0f) {
            pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
        }
        return;
    }

    // ── Taunt遷移: プレイヤーとの距離が fleeDistance 以上 ────
    if (pBaseEnemy_->CalcDistanceToPlayer() >= param.fleeDistance) {
        GetEnemy()->StartTaunt();
        return;
    }

    // ── プレイヤーから遠ざかる方向を計算 ─────────────────────
    Vector3 toPlayer        = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    toPlayer.y              = 0.0f;
    const float toPlayerLen = toPlayer.Length();
    if (toPlayerLen < 0.001f) {
        return;
    }
    toPlayer              = toPlayer * (1.0f / toPlayerLen);
    const Vector3 fleeDir = toPlayer * -1.0f;

    // プレイヤーから遠ざかれる方向を選ぶ。
    Vector3 pos = pBaseEnemy_->GetWorldPosition();
    pos.y       = 0.0f;

    const float rx = Field::baseScale_.x - pBaseEnemy_->GetParameter().baseScale_.x;
    const float rz = Field::baseScale_.z - pBaseEnemy_->GetParameter().baseScale_.z;

    // 壁との余裕距離（この距離以下になったら壁に近いと判定）
    const float wallMargin = param.fleeDistance * 0.5f;

    // 壁に近いかフラグ
    const bool nearWallPX = (rx - pos.x) < wallMargin; // +X壁
    const bool nearWallNX = (pos.x + rx) < wallMargin; // -X壁
    const bool nearWallPZ = (rz - pos.z) < wallMargin; // +Z壁
    const bool nearWallNZ = (pos.z + rz) < wallMargin; // -Z壁

    // fleeDirの角度を基点に、一定ステップで候補方向をサンプリング
    // プレイヤーから遠ざかる度合いが最大の有効方向を選ぶ
    const float baseAngle      = std::atan2(fleeDir.x, fleeDir.z);
    constexpr int kSamples     = 16;
    constexpr float kAngleStep = 3.14159265f * 2.0f / kSamples;

    Vector3 bestDir = fleeDir;
    float bestScore = -1e9f;

    for (int i = 0; i < kSamples; ++i) {
        // baseAngle に近い順
        const int offset        = (i % 2 == 0) ? (i / 2) : -(i / 2 + 1);
        const float angle       = baseAngle + offset * kAngleStep;
        const Vector3 candidate = {std::sin(angle), 0.0f, std::cos(angle)};

        // この方向が境界に向かっているかチェック
        if (nearWallPX && candidate.x > 0.0f) {
            continue;
        }
        if (nearWallNX && candidate.x < 0.0f) {
            continue;
        }
        if (nearWallPZ && candidate.z > 0.0f) {
            continue;
        }
        if (nearWallNZ && candidate.z < 0.0f) {
            continue;
        }

        // プレイヤーから遠ざかる度合い
        const float score = candidate.x * fleeDir.x + candidate.z * fleeDir.z;
        if (score > bestScore) {
            bestScore = score;
            bestDir   = candidate;
        }
    }

    // フィールド中心方向へ逃げる
    Vector3 moveDir = bestDir;
    if (bestScore <= -1e8f) {
        Vector3 toCenter = Vector3(0.0f, 0.0f, 0.0f) - pos;
        const float len  = toCenter.Length();
        moveDir          = (len > 0.001f) ? toCenter * (1.0f / len) : fleeDir;
    }

    // ── 方向転換ロック ────────────────────────────────────────
    const float kDirLockTime = 1.5f; // 方向を維持する秒数
    const float kSameDirDot  = 0.98f; // この内積以上なら同じ方向

    dirLockTimer_ -= dt;

    if (dirLockTimer_ > 0.0f) {
        // ロック中：前回の方向をそのまま使う
        moveDir = lockedMoveDir_;
    } else {
        // 前回と方向が変わったらロックをかける
        const float dot = moveDir.x * lockedMoveDir_.x + moveDir.z * lockedMoveDir_.z;
        if (dot < kSameDirDot) {
            dirLockTimer_ = kDirLockTime;
        }
        lockedMoveDir_ = moveDir;
    }

    // ── 進行方向を向いて移動 ─────────────────────────────────
    const float targetAngle = std::atan2(-moveDir.x, -moveDir.z);
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseRotationY(), targetAngle, 0.8f));
    pBaseEnemy_->AddPosition(moveDir * (param.fleeSpeed * dt));
}

void StrongEnemyFleeBehavior::Debug() {}