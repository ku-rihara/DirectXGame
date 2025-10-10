#pragma once
#include "Easing/Easing.h"
#include "utility/CameraEditor/CameraAnimation.h"
#include "utility/ShakeEditor/ShakePlayer.h"
#include "Vector3.h"

class GameCamera;

class CameraRendition {

public:
    CameraRendition()  = default;
    ~CameraRendition() = default;

    void Init();
    void Update(const float& speedRate);

    // play呼び出し
    void AnimationPlay(const std::string& filename);
    void ShakePlay(const std::string& filename);

private:
    GameCamera* pGameCamera_    = nullptr;
    std::unique_ptr<ShakePlayer> shakePlayer_;
    std::unique_ptr<CameraAnimation> cameraAnimation_;

public:
    
   const Vector3& GetShakeOffset() const { return shakePlayer_->GetTotalShakeOffset(); }

    // セッター
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }
    void SetViewProjection(ViewProjection* viewProjection);
};