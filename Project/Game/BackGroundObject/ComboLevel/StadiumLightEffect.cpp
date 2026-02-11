#include "StadiumLightEffect.h"
#include"utility/ObjectFromBlender/ObjectFromBlender.h"
#include "Frame/Frame.h"

void StadiumLightEffect::Init(const std::string& filename) {
    BaseComboLevelBackObj::Init(filename);

    ObjectFromBlender_->SetLoopEndCallback(static_cast<int32_t>(ObjEffectMode::PULSE),EasingAdaptTransform::Scale, [this]() {
        isPulseOneCycleEnd_ = true;
    });
}

void StadiumLightEffect::Update(float playSpeed) {
    isPulseCycleStart_ = true;
    BaseComboLevelBackObj::Update(playSpeed);
}
