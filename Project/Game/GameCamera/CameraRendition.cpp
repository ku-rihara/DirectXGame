#include "CameraRendition.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Shake.h"

void CameraRendition::Init() {
    currentShakeMode_    = ShakeMode::WAIT;

    // 各イージングの初期化
  /*  inEase_.Init("CameraZoomIn");
    outEase_.Init("CameraZoomOut");
    backlashEase_.Init("CameraBacklash");*/

    shakePlayer_ = std::make_unique<ShakePlayer>();
    cameraAnimation_ = std::make_unique<CameraAnimation>();

}

void CameraRendition::Update() {
    if (!pGameCamera_) {
        return;
    }

    shakePlayer_->Update(Frame::DeltaTimeRate());
    cameraAnimation_->Update(Frame::DeltaTimeRate());

    // Shake系統の更新
    int shakeModeIndex = static_cast<int>(currentShakeMode_);
    if (shakeModeIndex >= 0 && shakeModeIndex < static_cast<int>(ShakeMode::COUNT)) {
        (this->*spShakeFuncTable_[shakeModeIndex])();
    }

    //// Behavior系統の更新
    //int behaviorModeIndex = static_cast<int>(currentBehaviorMode_);
    //if (behaviorModeIndex >= 0 && behaviorModeIndex < static_cast<int>(BehaviorMode::COUNT)) {
    //    (this->*spBehaviorFuncTable_[behaviorModeIndex])();
    //}
}

// =================================== Shake系統の更新関数 ===================================

void CameraRendition::UpdateShakeWait() {
    // 待機
}

void CameraRendition::UpdateShaking() {
    switch (shakeStep_) {
    case ShakeStep::SHAKE:
        // シェイク処理
        pGameCamera_->SetShakePos(ShakeWave<Vector3>(shakeT_, 0.6f));
        shakeT_ -= Frame::DeltaTime();

        if (shakeT_ > 0.0f) {
            return;
        }

        shakeT_ = 0.0f;
        pGameCamera_->SetShakePos(Vector3::ZeroVector());
        shakeStep_ = ShakeStep::RETURN_ROOT;
        break;

    case ShakeStep::RETURN_ROOT:
        // 待機モードに戻る
        ChangeShakeMode(ShakeMode::WAIT);
        break;
    }
}

// =================================== Behavior系統の更新関数 ===================================

void CameraRendition::UpdateBehaviorRoot() {
  // 待機
}

void CameraRendition::UpdateZoomInOut() {
    //switch (zoomStep_) {
    //case ZoomStep::ZOOM_IN:
    //    ZoomShakeUpdate(); // ZoomInOut専用のShake処理
    //    inEase_.Update(Frame::DeltaTime());
    //    pGameCamera_->SetOffSet(tempInOffset_);

    //    // イージング完了チェック
    //    if (inEase_.IsFinished()) {
    //        zoomShakeT_ = 0.0f;
    //        pGameCamera_->SetShakePos(Vector3::ZeroVector());
    //        zoomStep_ = ZoomStep::ZOOM_OUT;
    //    }
    //    break;

    //case ZoomStep::ZOOM_OUT:
    //    outEase_.Update(Frame::DeltaTime());
    //    pGameCamera_->SetOffSet(tempInOffset_);

    //    // イージング完了チェック
    //    if (outEase_.IsFinished()) {
    //        saveOffset_ = tempInOffset_;
    //        zoomStep_   = ZoomStep::RETURN_ROOT;
    //    }
    //    break;

    //case ZoomStep::RETURN_ROOT:
    //    ChangeBehaviorMode(BehaviorMode::BACKLASH);
    //    break;
    //}
}

void CameraRendition::UpdateBacklash() {
    //switch (backlashStep_) {
    //case BacklashStep::OUT_IN:
    //    backlashEase_.Update(Frame::DeltaTimeRate());
    //    pGameCamera_->SetOffSet(tempBacklashOffset_);

    //    // イージング完了チェック
    //    if (backlashEase_.IsFinished()) {
    //        pGameCamera_->SetOffSet(tempBacklashOffset_);
    //        backlashStep_ = BacklashStep::RETURN_ROOT;
    //    }
    //    break;

    //case BacklashStep::RETURN_ROOT:
    //    ChangeBehaviorMode(BehaviorMode::ROOT);
    //    break;
    //}
}

// =================================== ヘルパー関数 ===================================

void CameraRendition::ZoomShakeUpdate() {
    //// ZoomInOut専用のShake処理
    //pGameCamera_->SetShakePos(Shake<Vector3>(zoomShakeT_, 1.6f));
    //zoomShakeT_ -= Frame::DeltaTime();

    //if (zoomShakeT_ > 0.0f) {
    //    return;
    //}
    //zoomShakeT_ = 0.0f;
    //pGameCamera_->SetShakePos(Vector3::ZeroVector());
}

// =================================== モード変更関数（Shake系統） ===================================

void CameraRendition::ChangeShakeMode(ShakeMode newMode) {
    currentShakeMode_ = newMode;

    // 新しいモードに応じて初期化処理を実行
    switch (newMode) {
    case ShakeMode::SHAKING:
        InitializeShaking();
        break;
    case ShakeMode::WAIT:
    default:
        // 待機モードは特に初期化不要
        break;
    }
}

void CameraRendition::ChangeToShaking() {
    if (currentShakeMode_ != ShakeMode::WAIT) {
        return;
    }
    ChangeShakeMode(ShakeMode::SHAKING);
}

void CameraRendition::ChangeToShakeWait() {
    ChangeShakeMode(ShakeMode::WAIT);
}

// =================================== モード変更関数（Behavior系統） ===================================

//void CameraRendition::ChangeBehaviorMode(BehaviorMode newMode) {
//    currentBehaviorMode_ = newMode;
//
//    // 新しいモードに応じて初期化処理を実行
//    switch (newMode) {
//    case BehaviorMode::ZOOM_IN_OUT:
//        InitializeZoomInOut();
//        break;
//    case BehaviorMode::BACKLASH:
//        InitializeBacklash();
//        break;
//    case BehaviorMode::ROOT:
//    default:
//
//        break;
//    }
//}

void CameraRendition::ChangeToZoomInOut() {
   /* if (currentBehaviorMode_ != BehaviorMode::ROOT)
        return;
    ChangeBehaviorMode(BehaviorMode::ZOOM_IN_OUT);*/
}

void CameraRendition::ChangeToBacklash() {
   /* ChangeBehaviorMode(BehaviorMode::BACKLASH);*/
}

void CameraRendition::ChangeToBehaviorRoot() {
   /* ChangeBehaviorMode(BehaviorMode::ROOT);*/
}

// =================================== 初期化関数 ===================================

void CameraRendition::InitializeShaking() {
    shakeStep_ = ShakeStep::SHAKE;
    shakeTMax_ = 0.8f;
    shakeT_    = shakeTMax_;
}

void CameraRendition::InitializeZoomInOut() {
   /* zoomStep_ = ZoomStep::ZOOM_IN;

    zoomShakeTMax_ = kWaitTime_ + 0.4f;
    zoomShakeT_    = zoomShakeTMax_;*/
}

void CameraRendition::InitializeBacklash() {
   /* backlashStep_ = BacklashStep::OUT_IN;

    backlashEase_.ApplyFromJson("CameraZoomOut.json");
    backlashEase_.SaveAppliedJsonFileName();
    backlashEase_.SetAdaptValue(&tempBacklashOffset_);
    backlashEase_.Reset();*/
}

 // play呼び出し
void CameraRendition::AnimationPlay(const std::string& filename) {
    cameraAnimation_->Play(filename);
}
void CameraRendition::ShakePlay(const std::string& filename) {
    shakePlayer_->Play(filename);
}

// Shake系統の関数ポインタテーブル
void (CameraRendition::* CameraRendition::spShakeFuncTable_[static_cast<int>(ShakeMode::COUNT)])() = {
    &CameraRendition::UpdateShakeWait, // WAIT
    &CameraRendition::UpdateShaking // SHAKING
};
