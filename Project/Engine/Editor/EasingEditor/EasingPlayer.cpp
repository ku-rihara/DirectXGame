#include "EasingPlayer.h"

using namespace KetaEngine;
#include "Vector2.h"
#include "Vector3.h"

template <typename T>
void EasingPlayer<T>::Init() {
    BaseEffectPlayer::Init();
}

template <typename T>
void EasingPlayer<T>::Update(float speedRate) {
    if (effectData_) {
        effectData_->Update(speedRate);
    }
}

template <typename T>
void EasingPlayer<T>::Play(const std::string& easingName, const std::string& categoryName) {
    currentEffectName_   = easingName;
    currentCategoryName_ = categoryName;

    effectData_.reset();
    effectData_ = CreateEffectData();

    effectData_->Init(easingName, categoryName);
    effectData_->LoadData();
    effectData_->Play();
}

template <typename T>
std::unique_ptr<BaseEffectData> EasingPlayer<T>::CreateEffectData() {
    return std::make_unique<EasingData<T>>();
}

template <typename T>
const T& EasingPlayer<T>::GetValue() const {
    if (auto* data = dynamic_cast<EasingData<T>*>(effectData_.get())) {
        return data->GetPreviewValue();
    }
    static T dummy = {};
    return dummy;
}

template class EasingPlayer<float>;
template class EasingPlayer<Vector2>;
template class EasingPlayer<Vector3>;
