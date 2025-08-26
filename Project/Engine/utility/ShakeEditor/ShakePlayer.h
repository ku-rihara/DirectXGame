// ShakePlayer.h
#pragma once
#include "ShakeData.h"
#include "Vector3.h"
#include <memory>
#include <string>
#include <unordered_map>

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
    void StopShake(const std::string& shakeName); 
    void StopAllShakes(); 

private:
    /// 全シェイクオフセットを計算
    void UpdateTotalShakeOffset();

private:
    std::unordered_map<std::string, std::unique_ptr<ShakeData>> shakeDataMap_;
    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};

public:
    Vector3 GetTotalShakeOffset() const { return totalShakeOffset_; }
    bool IsPlaying(const std::string& shakeName) const;
    bool IsAnyShakePlaying() const;
};
