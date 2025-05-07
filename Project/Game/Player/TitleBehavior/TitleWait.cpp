/// behavior
#include"TitleWait.h"
#include"TitleRightPunch.h"

#include"Player/PlayerBehavior/PlayerMove.h"

/// Player
#include"Player/Player.h"
#include"GameCamera/GameCamera.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
TitleWait::TitleWait(Player* player)
	: BaseTitleBehavior("TitleWait",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	pressEase_.maxTime = 0.3f;
	restEase_.maxTime = 0.25f;
	pressScale_ = 0.85f;
	step_ = STEP::PRESS; // 落ちる
}

TitleWait::~TitleWait() {

}

//更新
void TitleWait::Update() {
	switch (step_)
	{
	case TitleWait::STEP::PRESS:
		///-------------------------------------------------------------------------------------------------
		///潰れる
		/// -----------------------------------------------------------------------------------------------
		pressEase_.time += Frame::DeltaTime();
		pPlayer_->SetScaleY(EaseOutQuart(1.0f, pressScale_, pressEase_.time, pressEase_.maxTime));

		/// 次のステップ
		if (pressEase_.time < pressEase_.maxTime)break;
		pPlayer_->SetScaleY(pressScale_);
		pressEase_.time = 0.0f;
		step_ = STEP::RESTORATIVE;

		break;
	case TitleWait::STEP::RESTORATIVE:
		///-------------------------------------------------------------------------------------------------
		///戻る
		/// -----------------------------------------------------------------------------------------------
		restEase_.time += Frame::DeltaTime();
		pPlayer_->SetScaleY(EaseInQuart(pressScale_, 1.0f, restEase_.time, restEase_.maxTime));

		/// 次のステップ
		if (restEase_.time < restEase_.maxTime)break;
		pPlayer_->SetScaleY(1.0f);
		restEase_.time = 0.0f;

		step_ = STEP::PRESS;
		break;
	default:
		break;
	}
	
}


void  TitleWait::Debug() {
	ImGui::Text("TitleWait");
}