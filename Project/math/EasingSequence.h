#pragma once
#include "Easing.h"
#include <memory>
#include <vector>

template <typename T>
class EasingSequence {
public:
    void AddStep(const std::string& name,T*adaptValue);
    void AddStep(std::unique_ptr<Easing<T>> easing);

    void Reset();
    void Update(float deltaTime);

    const T& GetValue() const;

private:
    std::vector<std::unique_ptr<Easing<T>>> steps_;
    size_t currentStep_ = 0;
   /* bool finished_      = false;*/

public:
    const Easing<T>* GetCurrentEasing() const;
    void SetBaseValue(T value);


  
};