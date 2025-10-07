#include "StadiumLightEffect.h"
#include "Frame/Frame.h"

void StadiumLightEffect::Init(const std::string& filename) {
    BaseComboLevelBackObj::Init(filename);
}

void StadiumLightEffect::Update(const float& playSpeed) {
    isPulseCycleStart_ = true; 
    BaseComboLevelBackObj::Update(playSpeed);

    putObjForBlender_->SetLoopEndCallback(static_cast<int32_t>(ObjEffectMode::PULSE), [this]() {
        isPulseOneCycleEnd_ = true;
    });
}
