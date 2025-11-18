/// behavior
#include"EnemyDamageReactionRoot.h"
/// obj
#include"Enemy/Types/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyDamageReactionRoot::EnemyDamageReactionRoot(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDamageReactionRoot", boss) {

}

EnemyDamageReactionRoot::~EnemyDamageReactionRoot() {

}

void EnemyDamageReactionRoot::Update() {
	
}

void EnemyDamageReactionRoot::Debug() {
	

}

void EnemyDamageReactionRoot::SelectDamageActionBehaviorByAttack(const PlayerCollisionInfo& playerCollisionInfo) {

 }