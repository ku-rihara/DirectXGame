#include "EasingEditor.h"

using namespace KetaEngine;
#include <imgui.h>

template <typename T>
void EasingEditor<T>::Init(const std::string& typeName) {
    BaseEffectEditor<EasingData<T>>::Init(typeName);
}

template <typename T>
void EasingEditor<T>::Update(float speedRate) {
    BaseEffectEditor<EasingData<T>>::Update(speedRate);
}

template <typename T>
std::unique_ptr<EasingData<T>> EasingEditor<T>::CreateEffectData() {
    return std::make_unique<EasingData<T>>();
}

template <typename T>
void EasingEditor<T>::RenderSpecificUI() {
    ImGui::SeparatorText("Easing Editor");
    BaseEffectEditor<EasingData<T>>::RenderPlayBack();
}

template <typename T>
void EasingEditor<T>::PlaySelectedAnimation() {
    auto* selected = this->GetSelectedEffect();
    if (selected) {
        selected->Play();
    }
}

template <typename T>
std::string EasingEditor<T>::GetFolderName() const {
    return EasingData<T>::GetStaticFolderName();
}

template class EasingEditor<float>;
template class EasingEditor<Vector2>;
template class EasingEditor<Vector3>;
