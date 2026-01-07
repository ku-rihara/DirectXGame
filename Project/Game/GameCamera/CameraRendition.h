#pragma once
#include "Easing/Easing.h"
#include "Editor/CameraEditor/CameraAnimation.h"
#include "Editor/ShakeEditor/ShakePlayer.h"
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
    void Update(float speedRate);

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

    /// <summary>
    /// 注視点ターゲット設定
    /// </summary>
    /// <param name="target">ターゲットのWorldTransform</param>
    void SetLookAtTarget(const KetaEngine::WorldTransform* target);

private:
    GameCamera* pGameCamera_ = nullptr;
    std::unique_ptr<KetaEngine::ShakePlayer> shakePlayer_;
    std::unique_ptr<KetaEngine::CameraAnimation> cameraAnimation_;

public:
    // getter
    const Vector3& GetShakeOffset() const { return shakePlayer_->GetTotalShakeOffset(); }

    // setter
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }
    void SetViewProjection(KetaEngine::ViewProjection* viewProjection);
};