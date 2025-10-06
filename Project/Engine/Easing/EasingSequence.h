#pragma once
#include "EasingStep.h"
#include <cassert>
#include <memory>
#include <vector>

class EasingSequence {
public:
    template <typename T>
    void AddStep(const std::string& name, T* adaptValue);

    template <typename T>
    void AddStep(std::unique_ptr<Easing<T>> easing);

    void Reset();
    void Update(float deltaTime);

    template <typename T>
    void SetBaseValue(const T& value);

    const IEasingStep* GetCurrentStep() const;
    IEasingStep* GetCurrentStep();

    size_t GetCurrentIndex() const { return currentStep_; }
    size_t GetStepCount() const { return steps_.size(); }
    void SetLoop(bool loop) { loop_ = loop; }
    bool IsAllFinished() const;
    bool IsAllPlaying() const;

private:
    std::vector<std::unique_ptr<IEasingStep>> steps_;
    size_t currentStep_ = 0;
    bool loop_          = false;
};
