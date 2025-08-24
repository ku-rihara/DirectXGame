#pragma once
#include "Easing/Easing.h"
#include "Vector3.h"
#include"utility/ShakeEditor/ShakePlayer.h"
#include"utility/CameraEditor/CameraAnimation.h"

class GameCamera;

class CameraRendition {
public:
    // Shake系統のモード
    enum class ShakeMode {
        WAIT,
        SHAKING,
        COUNT
    };

  public:
    CameraRendition()  = default;
    ~CameraRendition() = default;

    void Init();
    void Update();

    // Shake系統のモード変更
    void ChangeShakeMode(ShakeMode newMode);
    void ChangeToShaking();
    void ChangeToShakeWait();

    // Behavior
    void ChangeToZoomInOut();
    void ChangeToBacklash();
    void ChangeToBehaviorRoot();

    // play呼び出し
    void AnimationPlay(const std::string& filename);
    void ShakePlay(const std::string& filename);

private:
    enum class ShakeStep {
        SHAKE,
        RETURN_ROOT
    };

    // 関数ポインタテーブル
    static void (CameraRendition::* spShakeFuncTable_[static_cast<int>(ShakeMode::COUNT)])();
  

    // Shake系統の更新関数群
    void UpdateShakeWait();
    void UpdateShaking();

    // Behavior系統の更新関数群
    void UpdateBehaviorRoot();
    void UpdateZoomInOut();
    void UpdateBacklash();

    // ヘルパー関数
    void ZoomShakeUpdate();
    void InitializeShaking();
    void InitializeZoomInOut();
    void InitializeBacklash();

private:
    // メンバ変数
    GameCamera* pGameCamera_    = nullptr;
    ShakeMode currentShakeMode_ = ShakeMode::WAIT;
    std::unique_ptr<ShakePlayer> shakePlayer_;
    std::unique_ptr<CameraAnimation> cameraAnimation_;

    // Shake関連
    ShakeStep shakeStep_ = ShakeStep::SHAKE;
    float shakeT_        = 0.0f;
    float shakeTMax_     = 0.8f;

public:
    // ゲッター
    ShakeMode GetCurrentShakeMode() const { return currentShakeMode_; }
    bool IsShakeWait() const { return currentShakeMode_ == ShakeMode::WAIT; }
    Vector3 GetShakeOffset() const { return shakePlayer_->GetTotalShakeOffset(); }

     // セッター
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }
};