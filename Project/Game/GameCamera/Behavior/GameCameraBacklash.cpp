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
	
	ease_.Init("CameraZoomOut");
  /*  ease_.ApplyFromJson("CameraZoomOut.json");
    ease_.SaveAppliedJsonFileName();*/
    ease_.SetAdaptValue(&tempOffset_);

	ease_.SetOnFinishCallback([this] {
        pGameCamera_->SetOffSet(tempOffset_);
        step_ = Step::RETURNROOT;
    });

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
        ease_.Update(Frame::DeltaTimeRate());
        pGameCamera_->SetOffSet(tempOffset_);
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