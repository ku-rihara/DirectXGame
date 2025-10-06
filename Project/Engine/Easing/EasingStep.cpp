#include "EasingStep.h"
#include"Vector2.h"
#include "Vector3.h"


template <typename T>
EasingStep<T>::EasingStep()
    : easing_(std::make_unique<Easing<T>>()) {}

template <typename T>
EasingStep<T>::EasingStep(std::unique_ptr<Easing<T>> easing)
    : easing_(std::move(easing)) {}

template <typename T>
void EasingStep<T>::SetAdaptValue(T* value) {
    easing_->SetAdaptValue(value);
}

template <typename T>
void EasingStep<T>::SetOnWaitEndCallback(const std::function<void()>& callback) {
    easing_->SetOnWaitEndCallback(callback); 
}

template <typename T>
void EasingStep<T>::SetBaseValue(const T& value) {
    easing_->SetBaseValue(value);
}

template <typename T>
void EasingStep<T>::ApplyFromJson(const std::string& fileName) {
    easing_->ApplyFromJson(fileName);
}

template <typename T>
void EasingStep<T>::Update(const float& deltaTime) {
    easing_->Update(deltaTime);
}

template <typename T>
void EasingStep<T>::Reset() {
    easing_->Reset();
}

template <typename T>
bool EasingStep<T>::IsFinished() const {
    return easing_->IsFinished();
}

 template <typename T>
bool EasingStep<T>::IsPlaying() const {
    return easing_->IsPlaying();
}


// 明示的インスタンス化
template class EasingStep<float>;
template class EasingStep<Vector2>;
template class EasingStep<Vector3>;
