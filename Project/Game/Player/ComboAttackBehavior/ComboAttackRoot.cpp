/// Combobehavior
#include"ComboAttackRoot.h"
#include"ComboAttackFirst.h"
#include"ComboAttackJumpFirst.h"

#include"Player/PlayerBehavior/PlayerJump.h"
#include"JoyState/JoyState.h"

/// Player
#include"Player/Player.h"

#include<imgui.h>


//初期化
ComboAttackRoot::ComboAttackRoot(Player* player)
	: BaseComboAattackBehavior("ComboAttackRoot", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------
	attackPatern_ = AttackPatern::NORMAL;


}

ComboAttackRoot ::~ComboAttackRoot() {

}

//更新
void ComboAttackRoot::Update() {

	///---------------------------------------------------------
	/// キー入力によるコンボ開始処理
	///---------------------------------------------------------

	JudgeAttackPatern();

	switch (attackPatern_)
	{
	case AttackPatern::NORMAL:
		if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackFirst>(pPlayer_));
		}
		else {
			NormalAttackChangeForJoyStick();
		}

		break;
	case AttackPatern::JUMP:
		if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackJumpFirst>(pPlayer_));
		}
		else {
			JumpAttackAttackChangeForJoyStick();
		}
		break;
	default:
		break;
	}

}

void ComboAttackRoot::NormalAttackChangeForJoyStick() {
	if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

	if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) return;

	pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackFirst>(pPlayer_));

}

void ComboAttackRoot::JumpAttackAttackChangeForJoyStick() {
	if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

	if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B))) return;

	pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackJumpFirst>(pPlayer_));

}

void ComboAttackRoot::JudgeAttackPatern() {
	/// 浮遊時のコンボ
	if (dynamic_cast<PlayerJump*>(pPlayer_->GetBehavior())) {
		attackPatern_ = AttackPatern::JUMP;
	}
	else {
		attackPatern_ = AttackPatern::NORMAL;
	}
}

void  ComboAttackRoot::Debug() {
	ImGui::Text("ComboAttackRoot");
}