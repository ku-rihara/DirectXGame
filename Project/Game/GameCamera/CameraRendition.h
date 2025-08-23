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

  
private:
    enum class ShakeStep {
        SHAKE,
        RETURN_ROOT
    };

    // 関数ポインタテーブル
    static void (CameraRendition::* spShakeFuncTable_[static_cast<int>(ShakeMode::COUNT)])();
  
    // メンバ変数
    GameCamera* pGameCamera_          = nullptr;
    ShakeMode currentShakeMode_       = ShakeMode::WAIT;
 
    // Shake関連
    ShakeStep shakeStep_ = ShakeStep::SHAKE;
    float shakeT_        = 0.0f;
    float shakeTMax_     = 0.8f;

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

    // セッター
    void SetGameCamera(GameCamera* camera) { pGameCamera_ = camera; }

    // ゲッター
    ShakeMode GetCurrentShakeMode() const { return currentShakeMode_; }
    bool IsShakeWait() const { return currentShakeMode_ == ShakeMode::WAIT; }
};