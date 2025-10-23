#pragma once
#include "ShakeData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// シェイクプレイヤー
/// </summary>
class ShakePlayer {
public:
    ShakePlayer()  = default;
    ~ShakePlayer() = default;

    void Init(); //< 初期化
    void StopShake(); //< シェイク停止

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// シェイクの再生
    /// </summary>
    /// <param name="shakeName">シェイク名</param>
    void Play(const std::string& shakeName);

private:
    void UpdateTotalShakeOffset(); //< 全シェイクオフセットの計算

private:
    std::unique_ptr<ShakeData> shakeData_;
    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

public:
    const Vector3& GetTotalShakeOffset() const { return totalShakeOffset_; }
};