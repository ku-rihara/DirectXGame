#include "StadiumLightEffect.h"
#include"utility/PutObjForBlender/putObjForBlender.h"
#include "Frame/Frame.h"

void StadiumLightEffect::Init(const std::string& filename) {
    BaseComboLevelBackObj::Init(filename);

    putObjForBlender_->SetLoopEndCallback(static_cast<int32_t>(ObjEffectMode::PULSE),EasingAdaptTransform::Scale, [this]() {
        isPulseOneCycleEnd_ = true;
    });
}

void StadiumLightEffect::Update(float playSpeed) {
    isPulseCycleStart_ = true;
    BaseComboLevelBackObj::Update(playSpeed);
}
