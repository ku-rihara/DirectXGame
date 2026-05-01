#include "ZakoCrawlBackwardsBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include "Player/Player.h"

ZakoCrawlBackwardsBehavior::ZakoCrawlBackwardsBehavior(NormalEnemy* enemy)
    : BaseEnemyBehavior("ZakoCrawlBackwardsBehavior", static_cast<BaseEnemy*>(enemy)), pNormalEnemy_(enemy) {

    // StumbleBackwards再生。終了後にCrawlBackwardsへ移行
    pNormalEnemy_->PlayNormalAnimation(NormalEnemy::NormalAnimationType::StumbleBackwards, false);
    pNormalEnemy_->SetIsInStumblePhase(true);

    const std::string stumbleName = "StumbleBackwards";
    pBaseEnemy_->GetAnimationObject()->SetAnimationEndCallback(stumbleName, [this]() {
        phase_ = Phase::CRAWL;
        pNormalEnemy_->SetIsInStumblePhase(false);
        pNormalEnemy_->PlayNormalAnimation(NormalEnemy::NormalAnimationType::CrawlBackwards, true);

    });
}

ZakoCrawlBackwardsBehavior::~ZakoCrawlBackwardsBehavior() {
    // Behavior破棄時にコールバックが残っていれば削除
    if (auto* anim = pBaseEnemy_->GetAnimationObject()) {
        anim->RemoveAnimationEndCallback("StumbleBackwards");
    }
    // Stumbleフェーズフラグを必ずクリア
    pNormalEnemy_->SetIsInStumblePhase(false);
}

void ZakoCrawlBackwardsBehavior::Update() {
    float deltaTime = KetaEngine::Frame::DeltaTimeRate();

    // プレイヤー方向を計算（両フェーズ共通）
    Vector3 toPlayer = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    toPlayer.y       = 0.0f;
    toPlayer.Normalize();

    // プレイヤーを向いたまま後退するため、正面はプレイヤー方向
    float targetAngle  = std::atan2(-toPlayer.x, -toPlayer.z);
    float currentAngle = pBaseEnemy_->GetBaseRotationY();
    pBaseEnemy_->SetRotationY(LerpShortAngle(currentAngle, targetAngle, 0.8f));

    // STUMBLEフェーズは向きのみ、移動なし
    if (phase_ == Phase::STUMBLE) {

        return;
    }

    // CRAWLフェーズ：アニメーションが途切れていたら再生し直す
    const std::string crawlName = "CrawlBackwards";
    if (pBaseEnemy_->GetAnimationObject()->GetCurrentAnimationName() != crawlName) {
        pNormalEnemy_->PlayNormalAnimation(NormalEnemy::NormalAnimationType::CrawlBackwards, true);
    }

    // CRAWLフェーズ：逃走時間制限チェック
    fleeTimer_ += deltaTime;
    if (fleeTimer_ >= pNormalEnemy_->GetNormalParameter().fleeTime) {
        return;
    }

    // プレイヤーと逆方向（後ろ）へ移動
    Vector3 fleeDir = toPlayer * -1.0f;
    pBaseEnemy_->AddPosition(fleeDir * (pNormalEnemy_->GetNormalParameter().fleeSpeed * deltaTime));
}

void ZakoCrawlBackwardsBehavior::Debug() {
}