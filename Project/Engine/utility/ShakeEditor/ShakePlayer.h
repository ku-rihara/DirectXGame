#pragma once
#include "ShakeData.h"
#include "Vector3.h"
#include <memory>
#include <string>

class ShakePlayer {
public:
    ShakePlayer()  = default;
    ~ShakePlayer() = default;

    /// 初期化
    void Init();

    /// 更新
    void Update(float deltaTime);

    /// シェイク制御
    void Play(const std::string& shakeName);
    void StopShake();
 
private:
    /// 全シェイクオフセットを計算
    void UpdateTotalShakeOffset();
private:
    std::unique_ptr<ShakeData> shakeData_; 
    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

public:
  
    Vector3 GetTotalShakeOffset() const { return totalShakeOffset_; }
};