
/// behavior
#include"EnemyRoot.h"
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

}

EnemySpawn::~EnemySpawn() {

}

void EnemySpawn::Update() {
	switch (step_)
	{
	case EnemySpawn::Step::SPAWN:

		spawnEasing_.time += Frame::DeltaTime();

		pBaseEnemy_->SetScale(
			EaseOutBack(Vector3::ZeroVector(), BaseEnemy::InitScale_,
				spawnEasing_.time, spawnEasing_.maxTime)
		);

		if (spawnEasing_.time < spawnEasing_.maxTime)break;
		spawnEasing_.time = spawnEasing_.maxTime;
		step_ = Step::ROOT;

		break;
	case EnemySpawn::Step::ROOT:
		pBaseEnemy_->ChangeMoveBehavior(std::make_unique<EnemyRoot>(pBaseEnemy_));
		break;
	default:
		break;
	}
	/// 生成処理
	


}

void EnemySpawn::Debug() {


}
