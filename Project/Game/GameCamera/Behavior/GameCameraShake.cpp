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
	
	 shakeTMax_ = 5.0f;
	 shakeT_ = shakeTMax_;
}

GameCameraShake::~GameCameraShake() {

}

//更新
void GameCameraShake::Update() {
	pGameCamera_->SetShakePos(Shake<Vector3>(shakeT_, 3.0f));
	shakeT_ -= Frame::DeltaTime();
	if (shakeT_ <= 0.0f) {
		shakeT_ = 0.0f;
		pGameCamera_->SetShakePos(Vector3::ZeroVector());
	}
}


void  GameCameraShake::Debug() {
	ImGui::Text("GameCameraShake");
}