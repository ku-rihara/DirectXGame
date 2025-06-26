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

	/*pressEase_.maxTime = 0.3f;
	restEase_.maxTime = 0.25f;
	pressScale_ = 0.85f;*/

	pressEase_.Init("TitleWait");
    pressEase_.ApplyFromJson("TitleWait.json");
    pressEase_.SaveAppliedJsonFileName();
    pressEase_.SetAdaptValue(&tempScaleY_);
    pressEase_.Reset();

    pressEase_.SetOnWaitEndCallback([this]() {
        pressEase_.Reset();
    });


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
        pressEase_.Update(Frame::DeltaTimeRate());
       /* pressEase_.UpdateWait(Frame::DeltaTimeRate());*/
        pPlayer_->SetScaleY(tempScaleY_);
		

		break;
	case TitleWait::STEP::RESTORATIVE:
		///-------------------------------------------------------------------------------------------------
		///戻る
		/// -----------------------------------------------------------------------------------------------
		

		step_ = STEP::PRESS;
		break;
	default:
		break;
	}
	
}


void  TitleWait::Debug() {
	ImGui::Text("TitleWait");
}