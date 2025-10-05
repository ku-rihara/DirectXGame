#include "BaseComboLevelBackObj.h"
#include "Frame/Frame.h"

void BaseComboLevelBackObj::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}

void BaseComboLevelBackObj::Update(const float& playSpeed) {

    if (effectMode_ == ObjEffectMode::NONE) {
        return;
    }

    putObjForBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(effectMode_) - 1);
}

void BaseComboLevelBackObj::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}

int32_t BaseComboLevelBackObj::ConvertEffectModeToInt(const ObjEffectMode& mode) {
    return static_cast<int32_t>(mode);
}