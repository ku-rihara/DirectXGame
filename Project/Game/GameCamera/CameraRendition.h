#pragma once
#include "Easing/Easing.h"
#include "utility/CameraEditor/CameraAnimation.h"
#include "utility/ShakeEditor/ShakePlayer.h"
#include "Vector3.h"

class GameCamera;

/// <summary>
/// カメラ演出制御クラス
/// </summary>
class CameraRendition {
public:
    CameraRendition()  = default;
    ~CameraRendition() = default;

    // 初期化、更新
    void Init();
    void Update(const float& speedRate);

    /// <summary>
    /// アニメーション再生
    /// </summary>
    /// <param name="filename">アニメーションファイル名</param>
    void AnimationPlay(const std::string& filename);

    /// <summary>
    /// シェイク再生
    /// </summary>
    /// <param name="filename">シェイクファイル名</param>
    void ShakePlay(const std::string& filename);

private:
    GameCamera* pGameCamera_ = nullptr;
    std::unique_ptr<ShakePlayer> shakePlayer_;
    std::unique_ptr<CameraAnimation> cameraAnimation_;

public:
    // getter
    const Vector3& GetShakeOffset() const { return shakePlayer_->GetTotalShakeOffset(); }

    // setter
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }
    void SetViewProjection(ViewProjection* viewProjection);
};