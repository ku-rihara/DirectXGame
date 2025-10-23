#pragma once
#include "DissolveData.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// ディゾルブ再生クラス
/// </summary>
class DissolvePlayer {
public:
    DissolvePlayer()  = default;
    ~DissolvePlayer() = default;

    void Init(); //< 初期化
    void StopDissolve(); //< ディゾルブ停止

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

    /// <summary>
    /// ディゾルブ再生
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    void Play(const std::string& dissolveName);

private:
    std::unique_ptr<DissolveData> dissolveData_;

public:
    bool IsDissolveEnabled() const { return dissolveData_->IsDissolveEnabled(); }
    bool IsPlaying() const { return dissolveData_ ? dissolveData_->IsPlaying() : false; }
};