#pragma once

#include <functional>
#include <memory>
#include <string>

namespace KetaEngine {

class BaseEffectData;

/// <summary>
/// エフェクト基底再生クラス
/// </summary>
class BaseEffectPlayer {
public:
    virtual ~BaseEffectPlayer() = default;

    //*----------------------------- public Methods -----------------------------*//

    virtual void Init()                                = 0;
    virtual void Update(float speedRate = 1.0f) = 0;
    virtual void Play(const std::string& effectName)   = 0;
    virtual void Stop();
    virtual void Reset();

    bool IsPlaying() const;
    bool IsFinished() const;
    bool IsReturning() const;

protected:
    //*---------------------------- protected Methods ----------------------------*//
    virtual std::unique_ptr<BaseEffectData> CreateEffectData() = 0;

protected:
    //*---------------------------- protected Variant ----------------------------*//
    std::unique_ptr<BaseEffectData> effectData_;
    std::string currentEffectName_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCurrentEffectName() const { return currentEffectName_; }
};

}; // KetaEngine
