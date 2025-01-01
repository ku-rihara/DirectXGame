/// behavior
#include"PlayerRoot.h"
#include"PlayerJump.h"

/// boss
#include"Player/Player.h"
/// frame
#include"Frame/Frame.h"
/// inupt
#include"JoyState/JoyState.h"
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

	pPlayer_->Move(pPlayer_->GetMoveSpeed());

	//　ジャンプに切り替え
	if (Input::GetInstance()->PushKey(DIK_J)) {
		pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
	}
	else {
		JumpForJoyState();//コントローラジャンプ
	}
}

void PlayerRoot::JumpForJoyState() {
	if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

	if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A))) return;

	pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
		
}

void  PlayerRoot::Debug() {
	ImGui::Text("Root");
	
}

