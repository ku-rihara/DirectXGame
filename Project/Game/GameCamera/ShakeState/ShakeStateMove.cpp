/// behavior
#include"ShakeStateMove.h"
#include"ShakeStateRoot.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include"Shake.h"
#include<imgui.h>


//初期化
ShakeStateMove::ShakeStateMove(GameCamera* gameCamera)
	: BaseShakeState("GameCameraShake", gameCamera) {

	/// ===================================================
	///変数初期化
	/// ===================================================

	shakeTMax_ = 0.8f;
	shakeT_ = shakeTMax_;
	step_ = Step::SHAKE;
}

ShakeStateMove::~ShakeStateMove() {

}

//更新
void ShakeStateMove::Update() {
	switch (step_)
	{
	case Step::SHAKE:
		/// ------------------------------------------------------
		/// シェイク
		///---------------------------------------------------------
		pGameCamera_->SetShakePos(ShakeWave<Vector3>(shakeT_, 0.6f));
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
		pGameCamera_->ChangeShakeState(std::make_unique<ShakeStateRoot>(pGameCamera_));
		break;
	default:
		break;
	}

}


void  ShakeStateMove::Debug() {
	ImGui::Text("GameCameraShake");
}