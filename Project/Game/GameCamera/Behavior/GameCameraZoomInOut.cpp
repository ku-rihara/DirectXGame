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

	inEase_.Init("CameraZoomIn");
    inEase_.ApplyFromJson("ZoomIn.json");
    inEase_.SaveAppliedJsonFileName();
    inEase_.SetAdaptValue(&tempinOffset_);

	outEase_.Init("CameraZoomOut");
    outEase_.ApplyFromJson("ZoomOut.json");
    outEase_.SaveAppliedJsonFileName();
    outEase_.SetAdaptValue(&tempinOffset_);

    inEase_.SetOnFinishCallback([this]() {
		pGameCamera_->SetOffSet(pGameCamera_->GetParamater().zoomOffset_);
 
    });

	 inEase_.SetOnWaitEndCallback([this]() {
        waitTime_ = kWaitTime_;
        shakeT_   = 0.0f;
        pGameCamera_->SetShakePos(Vector3::ZeroVector());
        step_ = Step::ZOOMOUT;
 
    });

	 outEase_.SetOnFinishCallback([this]() {
         saveOffset_ = pGameCamera_->GetOffset();
         step_       = Step::RETURNROOT;
     });

	 
	 /*inEase_.maxTime = 0.2f;*/
	 /*outEase_.maxTime = 0.2f;
	 kWaitTime_ = 0.2f;
	 timeOffset_ = 0.01f;*/
	 saveOffset_ = {};

	 shakeTMax_ = kWaitTime_+ 0.2f+0.2f;
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
        inEase_.Update(Frame::DeltaTime());
        pGameCamera_->SetOffSet(tempinOffset_);

		
		break;
		
		/// ------------------------------------------------------
		/// アウト
		///---------------------------------------------------------
	case Step::ZOOMOUT:
        outEase_.Update(Frame::DeltaTime());
        pGameCamera_->SetOffSet(tempinOffset_);

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