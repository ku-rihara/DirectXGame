/// behavior
#include"ComboAttackJumpFirst.h"


/// Player
#include"Player/Player.h"

#include<imgui.h>


//初期化
ComboAttackJumpFirst::ComboAttackJumpFirst(Player* player)
	: BaseComboAattackBehavior("ComboAttackJumpFirst",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------


	
}

ComboAttackJumpFirst::~ComboAttackJumpFirst() {

}

//更新
void ComboAttackJumpFirst::Update() {
	/*pPlayer_->Move(10.3f);*/
	//pPlayer_->Jump(speed_);
	
}


void  ComboAttackJumpFirst::Debug() {
	ImGui::Text("ComboAttackJumpFirst");
}