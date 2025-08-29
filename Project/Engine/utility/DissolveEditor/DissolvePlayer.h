#pragma once
#include "DissolveData.h"
#include "Vector3.h"
#include <memory>
#include <string>

class DissolvePlayer {
public:
    DissolvePlayer()  = default;
    ~DissolvePlayer() = default;

    /// 初期化
    void Init();

    /// 更新
    void Update(float deltaTime);

    /// ディゾルブ制御
    void Play(const std::string& dissolveName);
    void StopDissolve();

private:
    std::unique_ptr<DissolveData> dissolveData_;

public:
  
    bool IsDissolveEnabled() const { return  dissolveData_->IsDissolveEnabled(); }
    bool IsPlaying() const { return dissolveData_ ? dissolveData_->IsPlaying() : false; }
};