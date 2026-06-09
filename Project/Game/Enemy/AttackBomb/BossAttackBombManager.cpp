#include "BossAttackBombManager.h"
#include "../../GameCamera/GameCamera.h"
#include "DeathTimer/DeathTimer.h"
#include "Frame/Frame.h"
#include "Player/Player.h"

void BossAttackBombManager::Init() {
    postEffectPlayer_.Init();
}

void BossAttackBombManager::Update() {
    postEffectPlayer_.Update();

    if (!isActive_ || !pParam_) {
        return;
    }

    throwTimer_ += KetaEngine::Frame::DeltaTimeRate();

    if (throwTimer_ >= pParam_->GetThrowInterval()) {
        throwTimer_ = 0.0f;
        ThrowBomb();
    }

    for (auto it = bombs_.begin(); it != bombs_.end();) {
        (*it)->SetTargetPosition(playerPos_);
        (*it)->Update();
        if ((*it)->IsLanded()) {
            it = bombs_.erase(it);
        } else {
            ++it;
        }
    }
}

void BossAttackBombManager::Start() {
    isActive_   = true;
    throwTimer_ = pParam_ ? pParam_->GetThrowInterval() : 0.0f;
    bombs_.clear();
}

void BossAttackBombManager::Stop() {
    isActive_ = false;
    bombs_.clear();
}

void BossAttackBombManager::ThrowBomb() {
    auto bomb = std::make_unique<AttackBomb>();
    bomb->Init(bossPos_, playerPos_, pParam_->GetBombParam());

    float stressAmount = pParam_->GetStressPerBomb();
    bomb->SetOnLandedCallback([this, stressAmount]() {
        postEffectPlayer_.Play("BombLanding", "AttackBomb");

        if (pGameCamera_) {
            pGameCamera_->PlayShake("DamageCamera");
        }

        if (pDeathTimer_) {
            pDeathTimer_->TakeDamage(stressAmount);
        }

        if (pPlayer_) {
            pPlayer_->GetEffects()->Emit("StressEffect");
        }
    });

    bombs_.emplace_back(std::move(bomb));
}
