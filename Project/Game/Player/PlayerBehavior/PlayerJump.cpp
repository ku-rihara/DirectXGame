/// behavior
#include"PlayerJump.h"
#include"PlayerRoot.h"

/// boss
#include"Player/Player.h"
/// frame
#include"Frame/Frame.h"
#include<imgui.h>


//初期化
PlayerJump::PlayerJump(Player* player)
	: BasePlayerBehavior("PlayerJump",player) {

	/// ===================================================
	///変数初期化
	/// ===================================================

		speed_ = pPlayer_->GetMuzzulJumpSpeed();
	
}

PlayerJump ::~PlayerJump() {

}

//更新
void PlayerJump::Update() {
	pPlayer_->Move(0.3f);
	pPlayer_->Jump(speed_);
	
}


void  PlayerJump::Debug() {
	ImGui::Text("Jump");
}