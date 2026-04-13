#include "ComboLevelObjHolder.h"

void ComboLevelObjHolder::Update(float playSpeed) {
    for (auto& entry : entries_) {
        if (entry.obj) {
            entry.obj->Update(playSpeed);
        }
    }
}

void ComboLevelObjHolder::Add(const std::string& filename, int32_t comboLevel, bool isPulseMaster) {
    auto obj = std::make_unique<BaseComboLevelBackObj>();
    obj->Init(filename);

    if (isPulseMaster) {
        pulseMaster_ = obj.get();
        obj->SetIsPulseCycleStart(true);

        // PULSEが一周期終わったら他のオブジェクトのPULSEを開始
        obj->GetObjectFromBlender()->SetLoopEndCallback(
            static_cast<int32_t>(ObjEffectMode::PULSE),
            [this]() {
                for (auto& entry : entries_) {
                    if (entry.obj.get() != pulseMaster_) {
                        entry.obj->SetIsPulseCycleStart(true);
                    }
                }
            }
        );
    }

    entries_.push_back({ std::move(obj), comboLevel });
}

void ComboLevelObjHolder::CloseForComboLevel(int32_t level) {
    for (auto& entry : entries_) {
        if (entry.comboLevel <= level) {
            entry.obj->SetEffectMode(ObjEffectMode::CLOSE);
        }
    }
}

void ComboLevelObjHolder::SetEffectMode(int32_t comboLevel, const ObjEffectMode& mode) {
    for (auto& entry : entries_) {
        if (entry.comboLevel == comboLevel) {
            entry.obj->SetEffectMode(mode);
            // マスターが再スポーン時にPULSEを再開できるようにする
            if (mode == ObjEffectMode::SPAWN && entry.obj.get() == pulseMaster_) {
                entry.obj->SetIsPulseCycleStart(true);
            }
            return;
        }
    }
}

void ComboLevelObjHolder::EasingResetSelectGroup(int32_t groupNum) {
    for (auto& entry : entries_) {
        if (entry.obj) {
            entry.obj->EasingResetSelectGroup(groupNum);
        }
    }
}

void ComboLevelObjHolder::EasingAllReset() {
    for (auto& entry : entries_) {
        if (entry.obj) {
            entry.obj->EasingAllReset();
        }
    }
}

bool ComboLevelObjHolder::GetIsEasingFinish(int32_t comboLevel, int32_t groupNum) const {
    for (const auto& entry : entries_) {
        if (entry.comboLevel == comboLevel && entry.obj) {
            return entry.obj->GetIsEasingFinish(groupNum);
        }
    }
    return true;
}
