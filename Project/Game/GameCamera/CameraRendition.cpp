#include "CameraRendition.h"
#include "Frame/Frame.h"
#include "GameCamera/GameCamera.h"
#include "Shake.h"

void CameraRendition::Init() {

    shakePlayer_     = std::make_unique<KetaEngine::ShakePlayer>();
    cameraAnimation_ = std::make_unique<KetaEngine::CameraAnimation>();

    shakePlayer_->Init();
    cameraAnimation_->Init();
}

void CameraRendition::Update(float speedRate) {
    if (!pGameCamera_) {
        return;
    }

    shakePlayer_->Update();
    cameraAnimation_->Update(speedRate);
}

// play呼び出し
void CameraRendition::AnimationPlay(const std::string& filename) {
    cameraAnimation_->Play(filename);
    pGameCamera_->Reset();

    // GameCameraからターゲットを取得して設定
    if (pGameCamera_ && pGameCamera_->GetTarget()) {
        cameraAnimation_->SetLookAtTarget(pGameCamera_->GetTarget());
    }
}

void CameraRendition::ShakePlay(const std::string& filename) {
    shakePlayer_->Play(filename);
}

void CameraRendition::SetViewProjection(KetaEngine::ViewProjection* viewProjection) {
    cameraAnimation_->SetViewProjection(viewProjection);
}

void CameraRendition::SetLookAtTarget(const KetaEngine::WorldTransform* target) {
    if (cameraAnimation_) {
        cameraAnimation_->SetLookAtTarget(target);
    }
}