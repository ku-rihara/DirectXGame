/// behavior
#include"ComboAttackRoot.h"


/// Player
#include"Player/Player.h"

#include<imgui.h>


//初期化
ComboAttackRoot::ComboAttackRoot(Player* player)
	: BaseComboAattackBehavior("ComboAttackRoot", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------



}

ComboAttackRoot ::~ComboAttackRoot() {

}

//更新
void ComboAttackRoot::Update() {
	/*pPlayer_->Move(10.3f);*/
	//pPlayer_->Jump(speed_);

}


void  ComboAttackRoot::Debug() {
	ImGui::Text("ComboAttackRoot");
}