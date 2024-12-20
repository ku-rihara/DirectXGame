/// behavior
#include"ComboAttackSecond.h"

/// Player
#include"Player/Player.h"

#include<imgui.h>


//初期化
ComboAttackSecond::ComboAttackSecond(Player* player)
	: BaseComboAattackBehavior("ComboAttackSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------



}

ComboAttackSecond::~ComboAttackSecond() {

}

//更新
void ComboAttackSecond::Update() {
	/*pPlayer_->Move(10.3f);*/
	//pPlayer_->Jump(speed_);

}


void  ComboAttackSecond::Debug() {
	ImGui::Text("ComboAttackSecond");
}