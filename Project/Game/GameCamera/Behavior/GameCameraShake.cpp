/// behavior
#include"GameCameraRoot.h"
#include"GameCameraShake.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include"Shake.h"
#include<imgui.h>


//初期化
GameCameraShake::GameCameraShake(GameCamera* gameCamera)
	: BaseGameCameraBehavior("GameCameraShake", gameCamera) {

	/// ===================================================
	///変数初期化
	/// ===================================================
	
	 shakeTMax_ = 0.7f;
	 shakeT_ = shakeTMax_;
	 step_ = Step::SHAKE;
}

GameCameraShake::~GameCameraShake() {

}

//更新
void GameCameraShake::Update() {
	switch (step_)
	{
	case Step::SHAKE:
		/// ------------------------------------------------------
		/// シェイク
		///---------------------------------------------------------
		pGameCamera_->SetShakePos(Shake<Vector3>(shakeT_, 1.3f));
		shakeT_ -= Frame::DeltaTime();

		if (shakeT_ > 0.0f) return;
			shakeT_ = 0.0f;
			pGameCamera_->SetShakePos(Vector3::ZeroVector());
			step_ = Step::RETURNROOT;
		
		break;
	case Step::RETURNROOT:
		/// ------------------------------------------------------
		/// 通常モードに戻る
		///---------------------------------------------------------
		pGameCamera_->ChangeBehavior(std::make_unique<GameCameraRoot>(pGameCamera_));
		break;
	default:
		break;
	}
	
}


void  GameCameraShake::Debug() {
	ImGui::Text("GameCameraShake");
}