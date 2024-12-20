/// Combobehavior
#include"ComboAttackRoot.h"
#include"ComboAttackFirst.h"
#include"ComboAttackJumpFirst.h"

#include"Player/PlayerBehavior/PlayerJump.h"
#include"input/Input.h"

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

	///---------------------------------------------------------
	/// キー入力によるコンボ開始処理
	///---------------------------------------------------------

	/// 浮遊時のコンボ
	if (dynamic_cast<PlayerJump*>(pPlayer_->GetBehavior())) {
		if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackJumpFirst>(pPlayer_));
		}
	}
	//　通常のコンボ
	else {
		if (Input::GetInstance()->TrrigerKey(DIK_H)) {

			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackFirst>(pPlayer_));
		}
	}
}


void  ComboAttackRoot::Debug() {
	ImGui::Text("ComboAttackRoot");
}