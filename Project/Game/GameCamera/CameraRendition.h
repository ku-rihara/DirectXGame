#pragma once
#include "Easing/Easing.h"
#include "Vector3.h"

class GameCamera;

class CameraRendition {
public:
    // Shake系統のモード
    enum class ShakeMode {
        WAIT,
        SHAKING,
        COUNT
    };

    // Behavior系統のモード
    enum class BehaviorMode {
        ROOT,
        ZOOM_IN_OUT,
        BACKLASH,
        COUNT
    };

private:
    enum class ShakeStep {
        SHAKE,
        RETURN_ROOT
    };

    enum class ZoomStep {
        ZOOM_IN,
        ZOOM_OUT,
        RETURN_ROOT
    };

    enum class BacklashStep {
        OUT_IN,
        RETURN_ROOT
    };

    // 関数ポインタテーブル（Shake系統とBehavior系統を分離）
    static void (CameraRendition::* spShakeFuncTable_[static_cast<int>(ShakeMode::COUNT)])();
    static void (CameraRendition::* spBehaviorFuncTable_[static_cast<int>(BehaviorMode::COUNT)])();

    // メンバ変数
    GameCamera* pGameCamera_          = nullptr;
    ShakeMode currentShakeMode_       = ShakeMode::WAIT;
    BehaviorMode currentBehaviorMode_ = BehaviorMode::ROOT;

    // Shake関連
    ShakeStep shakeStep_ = ShakeStep::SHAKE;
    float shakeT_        = 0.0f;
    float shakeTMax_     = 0.8f;

    // ZoomInOut関連
    ZoomStep zoomStep_     = ZoomStep::ZOOM_IN;
    float zoomShakeT_      = 0.0f;
    float zoomShakeTMax_   = 0.0f;
    const float kWaitTime_ = 0.4f;
    Easing<Vector3> inEase_;
    Easing<Vector3> outEase_;
    Vector3 tempInOffset_;
    Vector3 saveOffset_;

    // Backlash関連
    BacklashStep backlashStep_ = BacklashStep::OUT_IN;
    Easing<Vector3> backlashEase_;
    Vector3 tempBacklashOffset_;

    // Shake系統の更新関数群
    void UpdateShakeWait();
    void UpdateShaking();

    // Behavior系統の更新関数群
    void UpdateBehaviorRoot();
    void UpdateZoomInOut();
    void UpdateBacklash();

    // ヘルパー関数
    void ZoomShakeUpdate(); // ZoomInOut内でのShake処理
    void InitializeShaking();
    void InitializeZoomInOut();
    void InitializeBacklash();

public:
    CameraRendition()  = default;
    ~CameraRendition() = default;

    void Init();
    void Update();

    // Shake系統のモード変更
    void ChangeShakeMode(ShakeMode newMode);
    void ChangeToShaking();
    void ChangeToShakeWait();

    // Behavior系統のモード変更
    void ChangeBehaviorMode(BehaviorMode newMode);
    void ChangeToZoomInOut();
    void ChangeToBacklash();
    void ChangeToBehaviorRoot();

    // セッター
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }

    // ゲッター
    ShakeMode GetCurrentShakeMode() const { return currentShakeMode_; }
    BehaviorMode GetCurrentBehaviorMode() const { return currentBehaviorMode_; }
    bool IsShakeWait() const { return currentShakeMode_ == ShakeMode::WAIT; }
    bool IsBehaviorRoot() const { return currentBehaviorMode_ == BehaviorMode::ROOT; }
};