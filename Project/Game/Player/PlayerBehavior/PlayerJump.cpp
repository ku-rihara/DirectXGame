/// behavior
#include"PlayerJump.h"
#include"PlayerMove.h"

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

	speed_ = pPlayer_->GetParamater().normalJump.jumpSpeed;
	
}

PlayerJump ::~PlayerJump() {

}

//更新
void PlayerJump::Update() {
	pPlayer_->Move(pPlayer_->GetParamater().moveSpeed);
	pPlayer_->Jump(speed_,pPlayer_->GetParamater().normalJump.fallSpeedLimit,pPlayer_->GetParamater().normalJump.gravity);
	
	if (pPlayer_->GetTransform().translation_.y > pPlayer_->GetParamater().startPos_.y) return;
	pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));

}


void  PlayerJump::Debug() {
	ImGui::Text("Jump");
}