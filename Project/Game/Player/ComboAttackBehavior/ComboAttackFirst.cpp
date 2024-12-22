/// behavior
#include"ComboAttackFirst.h"


/// Player
#include"Player/Player.h"

#include<imgui.h>


//初期化
ComboAttackFirst::ComboAttackFirst(Player* player)
	: BaseComboAattackBehavior("ComboAttackFirst",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------
	startPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
	targetPos_ = pPlayer_->GetRightHand()->GetTransform().LookAt(Vector3());

	
}

ComboAttackFirst::~ComboAttackFirst() {

}

//更新
void ComboAttackFirst::Update() {
	/*pPlayer_->Move(10.3f);*/
	//pPlayer_->Jump(speed_);

	// 最初のコンボの動き
	
	
}


void  ComboAttackFirst::Debug() {
	ImGui::Text("ComboAttackFirst");
}