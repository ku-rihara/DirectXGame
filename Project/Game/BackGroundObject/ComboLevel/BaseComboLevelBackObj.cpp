#include "BaseComboLevelBackObj.h"

void BaseComboLevelBackObj::Init(const std::string& filename) {
    BaseBackGroundObject::Init(filename);
    ObjectFromBlender_->EasingResetSelectGroup(static_cast<int32_t>(ObjEffectMode::SPAWN));
    // 初期は非表示に設定
    ObjectFromBlender_->SetAllObjectsScaleZero();
    // CLOSEアニメーション終了の瞬間にscale_を0にする
    ObjectFromBlender_->SetGroupEndCallback(
        static_cast<int32_t>(ObjEffectMode::CLOSE),
        [this]() { ObjectFromBlender_->SetAllObjectsScaleZeroNoUpdate(); });
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

        ObjectFromBlender_->EasingUpdateSelectGroup(playSpeed, static_cast<int32_t>(ObjEffectMode::SPAWN));

        // スポーンが終わったらPULSEへ
        if (ObjectFromBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::SPAWN))) {
            ObjectFromBlender_->EasingAllReset();
            effectMode_ = ObjEffectMode::PULSE;
        }
    }

    if (effectMode_ == ObjEffectMode::CLOSE) {
        // クローズが終わったらNONEへ
        if (ObjectFromBlender_->GetIsEasingFinish(static_cast<int32_t>(ObjEffectMode::CLOSE))) {
            EasingAllReset();
            isPulseCycleStart_ = false;
            // イージングリセット後にスケール0を適用して非表示状態を維持
            ObjectFromBlender_->SetAllObjectsScaleZero();
            effectMode_ = ObjEffectMode::NONE;
            return; // NONEになったので終了
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
        ObjectFromBlender_->EasingResetSelectGroup(static_cast<int32_t>(ObjEffectMode::SPAWN));
    }
}

bool BaseComboLevelBackObj::GetIsEasingFinish(int32_t groupNum) const {
    if (ObjectFromBlender_) {
        return ObjectFromBlender_->GetIsEasingFinish(groupNum);
    }
    return true;
}