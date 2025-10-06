#include "BaseComboLevelBackObj.h"
#include "Frame/Frame.h"

void BaseComboLevelBackObj::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
}

void BaseComboLevelBackObj::Update(const float& playSpeed) {

    if (effectMode_ == ObjEffectMode::NONE) {
        return;
    }

   
    if (effectMode_ == ObjEffectMode::SPAWN) {

         // クローズが終わってなかったらCLose優先
        if (putObjForBlender_->GetIsEasingPlaying(static_cast<int32_t>(ObjEffectMode::CLOSE) - kEffectModeOffset_)) {
            putObjForBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(ObjEffectMode::CLOSE) - kEffectModeOffset_);
            return;
        }

        // スポーンが終わったらPULSEへ
        if (putObjForBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::SPAWN) - kEffectModeOffset_)) {
            effectMode_ = ObjEffectMode::PULSE;
        }
      
    }
   
    putObjForBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(effectMode_) - kEffectModeOffset_);
}

void BaseComboLevelBackObj::Draw(const ViewProjection& viewProjection) {
    putObjForBlender_->DrawAll(viewProjection);
}

int32_t BaseComboLevelBackObj::ConvertEffectModeToInt(const ObjEffectMode& mode) {
    return static_cast<int32_t>(mode);
}


  void BaseComboLevelBackObj::EasingResetSelectGroup(const int32_t& groupNum) {
    if (putObjForBlender_) {
        putObjForBlender_->EasingResetSelectGroup(groupNum);
    }
}

void BaseComboLevelBackObj::EasingAllReset() {
    if (putObjForBlender_) {
        putObjForBlender_->EasingAllReset();
    }
}

bool BaseComboLevelBackObj::GetIsEasingFinish(const int32_t& groupNum) const {
    if (putObjForBlender_) {
        return putObjForBlender_->GetIsEasingFinish(groupNum);
    }
    return true;
}