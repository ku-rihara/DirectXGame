/// behavior
#include"GameCameraRoot.h"
#include"GameCameraZoomInOut.h"
#include"GameCameraBacklash.h"
/// boss
#include"GameCamera/GameCamera.h"
/// frame
#include"Frame/Frame.h"
#include"Shake.h"
#include<imgui.h>


//初期化
GameCameraZoomInOut::GameCameraZoomInOut(GameCamera* gameCamera)
	: BaseGameCameraBehavior("GameCameraZoomInOut", gameCamera) {

	/// ===================================================
	///変数初期化
	/// ===================================================

	 
	 inEase_.maxTime = 0.2f;
	 outEase_.maxTime = 0.2f;
	 kWaitTime_ = 0.2f;
	 timeOffset_ = 0.01f;
	 saveOffset_ = {};

	 shakeTMax_ = kWaitTime_+ inEase_.maxTime+0.2f;
	 shakeT_ = shakeTMax_;

	 step_ = Step::ZOOMIN;
}

GameCameraZoomInOut::~GameCameraZoomInOut() {

}

//更新
void GameCameraZoomInOut::Update() {
	switch (step_)
	{

		/// --------------------------------------------------------
		/// イン
		///---------------------------------------------------------
	case Step::ZOOMIN:
		ShakeUpdate();
		inEase_.time += Frame::DeltaTime();
		
		/// いージんぐ適応
		pGameCamera_->SetOffSet(EaseInCubic(pGameCamera_->GetParamater().firstOffset_, pGameCamera_->GetParamater().zoomOffset_,
			                                inEase_.time, inEase_.maxTime));

		// next
		if (inEase_.time < inEase_.maxTime)break;	
		pGameCamera_->SetOffSet(pGameCamera_->GetParamater().zoomOffset_);
		inEase_.time = inEase_.maxTime;
			step_ = Step::WAIT;
		
		break;
		/// --------------------------------------------------------
		/// イン
		///---------------------------------------------------------
	case Step::WAIT:
		ShakeUpdate();
		waitTime_ += Frame::DeltaTime();

		// next
		if (waitTime_ < kWaitTime_)break;
		waitTime_ = kWaitTime_;
		shakeT_ = 0.0f;
		pGameCamera_->SetShakePos(Vector3::ZeroVector());
		step_ = Step::ZOOMOUT;

		break;
		/// ------------------------------------------------------
		/// アウト
		///---------------------------------------------------------
	case Step::ZOOMOUT:
		outEase_.time += Frame::DeltaTime();

		/// いージんぐ適応
		pGameCamera_->SetOffSet(EaseInCubic(pGameCamera_->GetParamater().zoomOffset_, pGameCamera_->GetParamater().firstOffset_,
			outEase_.time, outEase_.maxTime));

		// next
		if (outEase_.time < outEase_.maxTime- timeOffset_)break;
		saveOffset_ = pGameCamera_->GetOffset();
		step_ = Step::RETURNROOT;
	

		break;
		/// ------------------------------------------------------
		/// 通常モードに戻る
		///---------------------------------------------------------
	case Step::RETURNROOT:
	
		pGameCamera_->ChangeBehavior(std::make_unique<GameCameraBackLash>(pGameCamera_));
		break;
	default:
		break;
	}
	
}

void GameCameraZoomInOut::ShakeUpdate() {
	pGameCamera_->SetShakePos(Shake<Vector3>(shakeT_, 1.6f));
	shakeT_ -= Frame::DeltaTime();

	if (shakeT_ > 0.0f) return;
	shakeT_ = 0.0f;
	pGameCamera_->SetShakePos(Vector3::ZeroVector());
}

void  GameCameraZoomInOut::Debug() {
	ImGui::Text("GameCameraShake");
}