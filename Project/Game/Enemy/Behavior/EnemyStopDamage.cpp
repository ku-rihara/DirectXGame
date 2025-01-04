
/// behavior
#include"EnemyStopDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyStopDamage::EnemyStopDamage(BaseEnemy* boss)
	: BaseEnemyBehaivor("EnemyStopDamage", boss) {

	// 赤色
	pBaseEnemy_->SetColor(Vector4(0.9f, 0, 0, 0.9f));


}

EnemyStopDamage::~EnemyStopDamage() {

}

void EnemyStopDamage::Update() {
	

	
	
	
}

void EnemyStopDamage::Debug() {
	

}
