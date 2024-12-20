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


	
}

ComboAttackFirst::~ComboAttackFirst() {

}

//更新
void ComboAttackFirst::Update() {
	/*pPlayer_->Move(10.3f);*/
	//pPlayer_->Jump(speed_);
	
}


void  ComboAttackFirst::Debug() {
	ImGui::Text("ComboAttackFirst");
}