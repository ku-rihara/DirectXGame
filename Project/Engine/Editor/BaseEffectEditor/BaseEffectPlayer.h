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

    virtual void Init()                         = 0;
    virtual void Update(float speedRate = 1.0f) = 0;

    // 再生、ストップ、リセット
    virtual void Play([[maybe_unused]] const std::string& effectName, [[maybe_unused]] const std::string& categoryName = "Common");
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
    std::string currentCategoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    const std::string& GetCurrentEffectName() const { return currentEffectName_; }
};

}; // KetaEngine
