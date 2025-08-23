#include "CameraAnimation.h"


void CameraAnimation::Init() {
    // 生成
    animationData_ = std::make_unique<CameraAnimationData>();

    // 初期状態に設定
    currentStep_ = AnimationState::IDLE;
    resetTimer_ = 0.0f;

    // オフセット値を初期化
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;
  
}

void CameraAnimation::Update(float deltaTime) {

    // 関数ポインタ呼び出し
    (this->*spFuncTable_[static_cast<size_t>(currentStep_)])();


    // アニメーションデータの更新
    if (animationData_) {
        animationData_->Update(deltaTime);
    }
}

void CameraAnimation::Play(const std::string& animationName) {
    if (!animationData_) {
        return;
    }

    currentAnimationName_ = animationName;
    currentStep_ = AnimationState::LOADING;
   
}

void CameraAnimation::Reset() {
    if (animationData_) {
        animationData_->Reset();
    }

    currentStep_ = AnimationState::IDLE;
  
    // オフセット値をリセット
    currentOffsetPosition_ = {0.0f, 0.0f, 0.0f};
    currentOffsetRotation_ = {0.0f, 0.0f, 0.0f};
    currentOffsetFov_      = 0.0f;
}

void CameraAnimation::StepWait() {
    
}


void CameraAnimation::StepLoad() {
    // アニメーションデータの初期化とロード
    animationData_->Init(currentAnimationName_);
    animationData_->LoadData();

    // 現在の値を初期値として保存
    SaveInitialValues();

    // アニメーションデータをリセットして再生開始
    animationData_->Reset();
    animationData_->Play();

   currentStep_ = AnimationState::PLAYING;
}

void CameraAnimation::StepPlaying() {
    
}

void CameraAnimation::StepReset() {

}

void CameraAnimation::SaveInitialValues() {

    if(!pViewProjection_) {
        return;
    }
    
    initialParam_.position = pViewProjection_->translation_;
    initialParam_.rotation = pViewProjection_->rotation_;
    initialParam_.fov      = pViewProjection_->fovAngleY_;
}

void CameraAnimation::ApplyOffsetToViewProjection() {
    if (!animationData_ || !pViewProjection_) {
        return;
    }

    // アニメーションデータを一時的なViewProjectionに適用
    animationData_->ApplyToViewProjection(*pViewProjection_);
}

 void CameraAnimation::SetViewProjection(ViewProjection* viewProjection) { 
     pViewProjection_ = viewProjection; 
 } 

/// --------------------------------------------------------------------------------
// メンバ関数のポインタ配列
/// --------------------------------------------------------------------------------
void (CameraAnimation::* CameraAnimation::spFuncTable_[])(){
    &CameraAnimation::StepWait,
    &CameraAnimation::StepLoad,
    &CameraAnimation::StepPlaying,
    &CameraAnimation::StepReset,
};