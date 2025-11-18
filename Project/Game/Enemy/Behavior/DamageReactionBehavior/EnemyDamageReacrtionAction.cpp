#include"EnemyDamageReacrtionAction.h"
/// obj
#include"Enemy/Types/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyDamageReactionAction::EnemyDamageReactionAction(BaseEnemy* boss, EnemyDamageReactionData* reactionData)
    : BaseEnemyDamageReaction("EnemyDamageReactionAction", boss) {

    pReactionData_ = reactionData;
}

EnemyDamageReactionAction::~EnemyDamageReactionAction() {

}

void EnemyDamageReactionAction::Update() {
	
}

void EnemyDamageReactionAction::Debug() {
	

}

