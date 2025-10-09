#pragma once
#include "EasingStep.h"
#include <cassert>
#include <functional>
#include <memory>
#include <vector>

class EasingSequence {
public:
    EasingSequence()  = default;
    ~EasingSequence() = default;

    template <typename T>
    void AddStep(const std::string& name, T* adaptValue);

    template <typename T>
    void AddStep(std::unique_ptr<Easing<T>> easing);

    void Reset();
    void Update(const float& deltaTime);

    template <typename T>
    void SetBaseValue(const T& value);

    const IEasingStep* GetCurrentStep() const;
    IEasingStep* GetCurrentStep();

    bool IsAllFinished() const;
    bool IsAllPlaying() const;

private:
    std::vector<std::unique_ptr<IEasingStep>> steps_;
    std::function<void()> onAllFinishCallback_ = nullptr;
    size_t currentStep_                        = 0;
    bool loop_                                 = false;

public:
    const size_t& GetCurrentIndex() const { return currentStep_; }
    const size_t& GetStepCount() const { return steps_.size(); }
    void SetLoop(const bool& loop) { loop_ = loop; }
    void SetOnAllFinishCallback(std::function<void()> callBack) {onAllFinishCallback_ = callBack;}
};
