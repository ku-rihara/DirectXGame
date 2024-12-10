/// behavior
#include"PlayerJump.h"
#include"PlayerRoot.h"

/// boss
#include"Game/Player/Player.h"
/// frame
#include"myEngine/Frame/Frame.h"


//初期化
PlayerJump::PlayerJump(Player* player)
	: BasePlayerBehavior("PlayerJump",player) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	
	if (pPlayer_->GetIsCollisionDebris()) {
		speed_ = 0.0f;
		pPlayer_->SetIsCollisionDebris(false);
	}
	else {
		speed_ = pPlayer_->GetMuzzulJumpSpeed();
	}
	/*pPlayer_->SetJumpSpeed(1.5f);*/
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