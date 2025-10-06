#pragma once
#include "Easing.h"
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

class IEasingStep {
public:
    virtual ~IEasingStep()                                  = default;
    virtual void ApplyFromJson(const std::string& fileName) = 0;
    virtual void Update(float deltaTime)                    = 0;
    virtual void Reset()                                    = 0;
    virtual bool IsFinished() const                         = 0;
    virtual bool IsPlaying() const                          = 0;
};

template <typename T>
class EasingStep : public IEasingStep {
public:
    EasingStep();
    ~EasingStep() override = default;   

    explicit EasingStep(std::unique_ptr<Easing<T>> easing);

    void ApplyFromJson(const std::string& fileName) override;
    void Update(float deltaTime) override;
    void Reset() override;
    bool IsFinished() const override;
    bool IsPlaying() const override;

private:
    std::unique_ptr<Easing<T>> easing_;

public:
    void SetAdaptValue(T* value);
    void SetBaseValue(const T& value);
    void SetOnWaitEndCallback(const std::function<void()>& callback);
    const T& GetValue() const { return easing_->GetValue(); }
};