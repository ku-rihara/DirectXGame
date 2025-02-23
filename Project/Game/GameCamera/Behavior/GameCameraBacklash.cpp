/// behavior
#include"GameCameraBackLash.h"
#include"GameCameraRoot.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include<imgui.h>


//初期化
GameCameraBackLash::GameCameraBackLash(GameCamera* gameCamera)
	: BaseGameCameraBehavior("GameCameraBackLash", gameCamera) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	
	ease_.time = 0.0f;
	ease_.maxTime = pGameCamera_->GetParamater().backLashTime_;
	ease_.backRatio = pGameCamera_->GetParamater().backLashRatio_;
	step_ = Step::OUTIN;
}

GameCameraBackLash ::~GameCameraBackLash() {

}

//更新
void GameCameraBackLash::Update() {
	

	switch (step_)
	{
	/// --------------------------------------------------------------------
	/// カメラ移動
	/// --------------------------------------------------------------------
	case GameCameraBackLash::Step::OUTIN:
		ease_.time += Frame::DeltaTimeRate();

		/// いージんぐ適応
		pGameCamera_->SetOffSet(Back::OutCircZero(pGameCamera_->GetParamater().firstOffset_, pGameCamera_->GetParamater().backLashOffset_,
			ease_.time, ease_.maxTime, ease_.backRatio));

		// next
		if (ease_.time < ease_.maxTime)break;
		pGameCamera_->SetOffSet(pGameCamera_->GetParamater().firstOffset_);
		ease_.time = ease_.maxTime;
		step_ = Step::RETURNROOT;
		break;
		/// --------------------------------------------------------------------
		/// カメラ移動
		/// --------------------------------------------------------------------
	case GameCameraBackLash::Step::RETURNROOT:
		pGameCamera_->ChangeBehavior(std::make_unique<GameCameraRoot>(pGameCamera_));
		break;
	default:
		break;
	}
}


void  GameCameraBackLash::Debug() {
	ImGui::Text("GameCameraBackLash");
}