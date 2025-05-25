
/// behavior
#include"EnemyWait.h"
#include"EnemySpawn.h"

/// math
#include"MathFunction.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"/// プレイヤーの座標取得に使ってるので消さないこと
/// frame
#include"Frame/Frame.h"


//初期化
EnemySpawn::EnemySpawn(BaseEnemy* boss)
	: BaseEnemyMoveBehavior("EnemySpawn", boss) {

	///spawn
	spawnEasing_.time = 0.0f;
	spawnEasing_.maxTime = 0.8f;

	pBaseEnemy_->SpawnRenditionInit();

}

EnemySpawn::~EnemySpawn() {

}

void EnemySpawn::Update() {
	switch (step_)
	{
		///------------------------------------------------------------------
		/// 敵の生成アニメーション
		///------------------------------------------------------------------
	case EnemySpawn::Step::SPAWN:

		// time 加算
		spawnEasing_.time += Frame::DeltaTime();

		// イージング適応
		pBaseEnemy_->SetScale(
            EaseOutBack(Vector3::ZeroVector(), pBaseEnemy_->GetParamater().initScale_,
				spawnEasing_.time, spawnEasing_.maxTime)
		);

		// timeがmaxになるまで早期リターン
        if (spawnEasing_.time < spawnEasing_.maxTime) {
            break;
        }

        // 終了処理
		spawnEasing_.time = spawnEasing_.maxTime;
		step_ = Step::ChangeNextBehavior; // 次のステップ

		break;
        ///------------------------------------------------------------------
        ///  移動Behaviorに切り替え
        ///------------------------------------------------------------------
	case EnemySpawn::Step::ChangeNextBehavior:
		pBaseEnemy_->ChangeMoveBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
		break;
	default:
		break;
	}
	/// 生成処理
	


}

void EnemySpawn::Debug() {


}
