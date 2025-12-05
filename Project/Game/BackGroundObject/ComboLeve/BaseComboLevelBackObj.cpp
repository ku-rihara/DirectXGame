#include "BaseComboLevelBackObj.h"
#include "Frame/Frame.h"

void BaseComboLevelBackObj::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
    putObjForBlender_->EasingUpdateSelectGroup(0.0f, static_cast<int32_t>(ObjEffectMode::SPAWN));
}

void BaseComboLevelBackObj::Update(float playSpeed) {

    if (effectMode_ == ObjEffectMode::NONE) {
        return;
    }

    if (effectMode_ == ObjEffectMode::SPAWN) {

        // クローズが終わってなかったらCLose優先
        if (putObjForBlender_->GetIsEasingPlaying(static_cast<int32_t>(ObjEffectMode::CLOSE))) {
            putObjForBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(ObjEffectMode::CLOSE));
            return;
        }

        // スポーンが終わったらPULSEへ
        if (putObjForBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::SPAWN))) {
            putObjForBlender_->EasingAllReset();
            effectMode_ = ObjEffectMode::PULSE;
        }
    }

    if (effectMode_ == ObjEffectMode::CLOSE) {
        // クローズが終わったらNONEへ
        if (putObjForBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::CLOSE))) {
            putObjForBlender_->EasingAllReset();
            isPulseCycleStart_ = false;
            putObjForBlender_->EasingUpdateSelectGroup(0.0f, static_cast<int32_t>(ObjEffectMode::SPAWN));
            effectMode_ = ObjEffectMode::NONE;       
        }
    }

    // PULSEの開始タイミングを制御
    if (effectMode_ == ObjEffectMode::PULSE && !isPulseCycleStart_) {
        return;
    }

    putObjForBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(effectMode_));
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