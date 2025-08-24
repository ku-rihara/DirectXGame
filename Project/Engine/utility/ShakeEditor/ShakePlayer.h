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
    void PlayShake(const std::string& shakeName);
    void StopShake(const std::string& shakeName);
    void StopAllShakes();
    void AddShakeData(std::unique_ptr<ShakeData> shakeData);

    /// シェイクデータの登録
    void RegisterShakeData(const std::string& name, ShakeData* shakeData);

    /// 状態確認
    bool IsPlayingShake(const std::string& shakeName) const;
    bool IsAnyShakePlaying() const;

    /// シェイクオフセット取得
    Vector3 GetTotalShakeOffset() const { return totalShakeOffset_; }

private:
    /// 全シェイクオフセットを計算
    void UpdateTotalShakeOffset();

private:
    // シェイクデータの管理
    std::unordered_map<std::string, std::unique_ptr<ShakeData>> ownedShakeData_;
    std::unordered_map<std::string, ShakeData*> registeredShakeData_;

    // 合成されたシェイクオフセット
    Vector3 totalShakeOffset_ = {0.0f, 0.0f, 0.0f};
};