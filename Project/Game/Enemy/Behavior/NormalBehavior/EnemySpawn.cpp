
/// behavior
#include "EnemySpawn.h"
#include "EnemyWait.h"

/// math
#include "MathFunction.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h" /// プレイヤーの座標取得に使ってるので消さないこと
/// frame
#include "Frame/Frame.h"

// 初期化
EnemySpawn::EnemySpawn(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemySpawn", boss) {

    /// spawn

    spawnEasing_.Init("SpawnScaling.json");
    spawnEasing_.SetAdaptValue(&tempEnemyScale_);
    spawnEasing_.Reset();

    spawnEasing_.SetOnFinishCallback([this]() {
        step_ = Step::ChangeNextBehavior; // 次のステップ
    });

}

EnemySpawn::~EnemySpawn() {
}

void EnemySpawn::Update() {
    switch (step_) {
        ///------------------------------------------------------------------
        /// 敵の生成アニメーション
        ///------------------------------------------------------------------
    case EnemySpawn::Step::EFFECTEMIT:
        pBaseEnemy_->GetEnemyEffects()->Emit("SpawnEffect");

        step_ = Step::SPAWN; // 次のステップ
        break;
        ///------------------------------------------------------------------
        /// 敵の生成アニメーション
        ///------------------------------------------------------------------
    case EnemySpawn::Step::SPAWN:

        spawnEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
        pBaseEnemy_->SetScale(tempEnemyScale_);

            break;
        ///------------------------------------------------------------------
        ///  移動Behaviorに切り替え
        ///------------------------------------------------------------------
    case EnemySpawn::Step::ChangeNextBehavior:
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
        break;
    default:
        break;
    }
    /// 生成処理
}

void EnemySpawn::Debug() {
}
