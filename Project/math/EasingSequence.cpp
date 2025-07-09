#pragma once

#include "EasingSequence.h"
#include<cassert>

template <typename T>
void EasingSequence<T>::AddStep(const std::string& name, T* adaptValue) {
    steps_.emplace_back(std::make_unique<Easing<T>>());
    steps_.back()->ApplyFromJson(name);

    steps_.back()->SetAdaptValue(adaptValue);

    steps_.back()->SetOnWaitEndCallback([this]() {
        if (currentStep_ + 1 < steps_.size()) {
            ++currentStep_;
            steps_[currentStep_]->Reset();
        }
    });
}

template <typename T>
void EasingSequence<T>::AddStep(std::unique_ptr<Easing<T>> easing) {
    steps_.emplace_back(std::move(easing));
}

template <typename T>
void EasingSequence<T>::Reset() {
    currentStep_ = 0;
    for (auto& step : steps_) {
        step->Reset();
    }
 
}

template <typename T>
void EasingSequence<T>::Update(float deltaTime) {
    if (steps_.empty()) {
        return;
    }
    steps_[currentStep_]->Update(deltaTime);
}

template <typename T>
const T& EasingSequence<T>::GetValue() const {
    assert(!steps_.empty());
    return steps_[currentStep_]->GetValue();
}

template <typename T>
const Easing<T>* EasingSequence<T>::GetCurrentEasing() const {
    if (currentStep_ < steps_.size()) {
        return steps_[currentStep_].get();
    }
    return nullptr;
}

template <typename T>
void EasingSequence<T>::SetBaseValue(T value) {
    for (auto& step : steps_) {
        step->SetBaseValue(value);
    }
 }

template class EasingSequence<float>;
template class EasingSequence<Vector2>;
template class EasingSequence<Vector3>;