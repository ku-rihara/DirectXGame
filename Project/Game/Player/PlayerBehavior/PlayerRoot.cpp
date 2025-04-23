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
     waitEase_.maxTime = 0.4f;
    easeDirection_ = 1.0f;
	speed_ = pPlayer_->GetParamater().moveSpeed;

	animationStep_ = AnimationStep::INIT;
}

PlayerRoot ::~PlayerRoot() {

}

//更新
void PlayerRoot::Update() {

    // アニメーション
	MoveAnimation();
    WaitAnimation();


		if ((Input::IsPressPad(0, XINPUT_GAMEPAD_Y))) {
			pPlayer_->Move(pPlayer_->GetParamater().moveSpeed *2.4f);
		}
		else {
			pPlayer_->Move(pPlayer_->GetParamater().moveSpeed);
		}
	
	//　ジャンプに切り替え
	if (Input::GetInstance()->PushKey(DIK_J)) {
		pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
	}
	else {
		JumpForJoyState();//コントローラジャンプ
	}
}

void PlayerRoot::JumpForJoyState() {
	
	if (!(Input::IsTriggerPad(0, XINPUT_GAMEPAD_A))) return;

	pPlayer_->ChangeBehavior(std::make_unique<PlayerJump>(pPlayer_));
		
}

void PlayerRoot::MoveAnimation() {
    if (!pPlayer_->GetIsMoving())
        return;

    switch (animationStep_) {
    case AnimationStep::INIT:
        ///============================================================================
        /// 初期化
        ///============================================================================
        animationCollTime_ = 0.5f;
        moveEase_.time   = 0.0f;
        waitEase_.time   = 0.0f;
        moveEase_.maxTime  = 0.4f;
      
        animationStep_   = AnimationStep::UPDATE;
        break;
    case AnimationStep::UPDATE:

        ///============================================================================
        /// アニメーション
        ///============================================================================

        moveEase_.time += Frame::DeltaTime();
        if (moveEase_.time >= moveEase_.maxTime) {
            moveEase_.time = 0.0f;
            animationStep_ = AnimationStep::COOLING;
        }

         pPlayer_->SetScaleY(
            EaseInCubic<float>(Vector3::UnitVector().y,Vector3::UnitVector().y - 0.1f, moveEase_.time, moveEase_.maxTime));
        break;
    case AnimationStep::COOLING:
        ///============================================================================
        /// クールタイム
        ///============================================================================

        animationCollTime_ -= Frame::DeltaTime();
        if (animationCollTime_ <= 0.0f) {
            animationStep_ = AnimationStep::INIT;
        }

        break;
    }


}

void PlayerRoot::WaitAnimation() {

     if (pPlayer_->GetIsMoving())
        return;


    ///============================================================================
    /// 待機アニメーション
    ///============================================================================
    waitEase_.time += Frame::DeltaTime() * easeDirection_; // 方向に応じて時間を増減

    // タイムが1を超えたら逆方向に、0未満になったら進む方向に変更
    if (waitEase_.time >= waitEase_.maxTime) {
        waitEase_.time = waitEase_.maxTime;
        easeDirection_ = -1.0f; // 逆方向に切り替え
    }

    else if (waitEase_.time <= 0.0f) {
        waitEase_.time = 0.0f;
        easeDirection_ = 1.0f; // 進む方向に切り替え
    }

    pPlayer_->SetScale(
        EaseInCubic<Vector3>(Vector3::UnitVector(), {Vector3::UnitVector().x - 0.1f, Vector3::UnitVector().y - 0.1f, Vector3::UnitVector().z - 0.1f}, waitEase_.time, waitEase_.maxTime));
}

void  PlayerRoot::Debug() {
	ImGui::Text("Root");
	
}

