#include "BossAttackBombManager.h"
// Camera
#include "../../GameCamera/GameCamera.h"
// Audio
#include "Audio/Audio.h"
// StressGauge
#include "StressGauge/StressGauge.h"
// Frame
#include "Frame/Frame.h"
// Player
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
    for (auto& bomb : bombs_) {
        bomb->StopTrail();
    }
    bombs_.clear();
}

void BossAttackBombManager::ThrowBomb() {

    // ボムの生成と初期化
    auto bomb = std::make_unique<AttackBomb>();
    bomb->Init(bossPos_, playerPos_, pParam_->GetBombParam());
    //　投げる音の再生
    KetaEngine::Audio::GetInstance()->Play("BananaThrow.mp3", 0.3f);

    float stressAmount = pParam_->GetStressPerBomb();

    // 着地した時のコールバックを設定
    bomb->SetOnLandedCallback([this, stressAmount]() {
        postEffectPlayer_.Play("BombLanding", "AttackBomb");

        // カメラシェイク
        if (pGameCamera_) {
            pGameCamera_->PlayShake("DamageCamera");
        }
        // ストレスゲージを増やす
        if (pStressGauge_) {
            pStressGauge_->TakeDamage(stressAmount);
        }
        // エフェクトを再生
        if (pPlayer_) {
            pPlayer_->GetEffects()->Emit("StressEffect");
        }

        // ストレスUPの音を再生
        KetaEngine::Audio::GetInstance()->Play("StressUpSE.mp3",0.3f);
    });

    bombs_.emplace_back(std::move(bomb));
}
