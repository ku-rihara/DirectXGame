/// behavior
#include"PlayerRoot.h"
#include"PlayerJump.h"

/// boss
#include"Player/Player.h"
/// frame
#include"Frame/Frame.h"
/// inupt
#include"input/Input.h"
/// imgui
#include<imgui.h>

//初期化
PlayerRoot::PlayerRoot(Player* boss)
	: BasePlayerBehavior("PlayerRoot", boss) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	

}

PlayerRoot ::~PlayerRoot() {

}

//更新
void PlayerRoot::Update() {

	pPlayer_->Move(0.3f);

	if (Input::GetInstance()->PushKey(DIK_J)) {
		pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
	}
}


void  PlayerRoot::Debug() {
	ImGui::Text("Root");
	
}

