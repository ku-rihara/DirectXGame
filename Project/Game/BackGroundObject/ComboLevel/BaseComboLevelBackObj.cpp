#include "BaseComboLevelBackObj.h"
#include "Frame/Frame.h"

void BaseComboLevelBackObj::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
    ObjectFromBlender_->EasingUpdateSelectGroup(0.0f, static_cast<int32_t>(ObjEffectMode::SPAWN));
}

void BaseComboLevelBackObj::Update(float playSpeed) {

    if (effectMode_ == ObjEffectMode::NONE) {
        return;
    }

    if (effectMode_ == ObjEffectMode::SPAWN) {

        // クローズが終わってなかったらCLose優先
        if (ObjectFromBlender_->GetIsEasingPlaying(static_cast<int32_t>(ObjEffectMode::CLOSE))) {
            ObjectFromBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(ObjEffectMode::CLOSE));
            return;
        }

        // スポーンが終わったらPULSEへ
        if (ObjectFromBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::SPAWN))) {
            ObjectFromBlender_->EasingAllReset();
            effectMode_ = ObjEffectMode::PULSE;
        }
    }

    if (effectMode_ == ObjEffectMode::CLOSE) {
        // クローズが終わったらNONEへ
        if (ObjectFromBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::CLOSE))) {
            ObjectFromBlender_->EasingAllReset();
            isPulseCycleStart_ = false;
            ObjectFromBlender_->EasingUpdateSelectGroup(0.0f, static_cast<int32_t>(ObjEffectMode::SPAWN));
            effectMode_ = ObjEffectMode::NONE;       
        }
    }

    // PULSEの開始タイミングを制御
    if (effectMode_ == ObjEffectMode::PULSE && !isPulseCycleStart_) {
        return;
    }

    ObjectFromBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(effectMode_));
}

int32_t BaseComboLevelBackObj::ConvertEffectModeToInt(const ObjEffectMode& mode) {
    return static_cast<int32_t>(mode);
}

void BaseComboLevelBackObj::EasingResetSelectGroup(int32_t groupNum) {
    if (ObjectFromBlender_) {
        ObjectFromBlender_->EasingResetSelectGroup(groupNum);
    }
}

void BaseComboLevelBackObj::EasingAllReset() {
    if (ObjectFromBlender_) {
        ObjectFromBlender_->EasingAllReset();
    }
}

bool BaseComboLevelBackObj::GetIsEasingFinish(int32_t groupNum) const {
    if (ObjectFromBlender_) {
        return ObjectFromBlender_->GetIsEasingFinish(groupNum);
    }
    return true;
}