#include "ComboLevelObjHolder.h"
#include "StadiumLightEffect.h"
#include"SpeakerEffect.h"

void ComboLevelObjHolder::Init(const std::string& filename) {
    // 各オブジェクトの初期化
    for (auto& obj : objects_) {
        if (obj) {
            obj->Init(filename);
        }
    }
}

void ComboLevelObjHolder::Update(const float& playSpeed) {
    // 各オブジェクトの更新
    for (auto& obj : objects_) {
        if (obj) {
            obj->Update(playSpeed);

            // PULSEのタイミングを合わせる処理
            if (dynamic_cast<StadiumLightEffect*>(obj.get())) {
                StadiumLightEffect* stadiumLight = dynamic_cast<StadiumLightEffect*>(obj.get());
                if (stadiumLight->GetIsPulseOneCycleEnd()) {
                    // PULSEが一周期終了したら他のオブジェクトのPULSEを開始
                    for (auto& otherObj : objects_) {
                        if (otherObj && otherObj.get() != obj.get()) {
                            otherObj->SetIsPulseCycleStart(true);
                        }
                    }
                 
                    stadiumLight->SetIsPulseOneCycleEnd(false); // フラグリセット
                }
            }
        }
    }
}

void ComboLevelObjHolder::Add(const ComboLevelObjType& type, const std::string& filename) {
    size_t index = ToIndex(type);

    // typeに応じて適切なオブジェクトを生成
    switch (type) {
    case ComboLevelObjType::STADIUM_LIGHT:
        objects_[index] = std::make_unique<StadiumLightEffect>();
        break;
    case ComboLevelObjType::SPEAKER:
        objects_[index] = std::make_unique<SpeakerEffect>();
        break;
    default:
        break;
    }

    // 生成したオブジェクトを初期化
    if (objects_[index]) {
        objects_[index]->Init(filename);
    }
}

void ComboLevelObjHolder::EasingResetSelectGroup(const int32_t& groupNum) {
    for (auto& obj : objects_) {
        if (obj) {
            obj->EasingResetSelectGroup(groupNum);
        }
    }
}

void ComboLevelObjHolder::EasingAllReset() {
    for (auto& obj : objects_) {
        if (obj) {
            obj->EasingAllReset();
        }
    }
}

void ComboLevelObjHolder::CloseForComboLevel(const int32_t& level) {
    // レベルに応じたオブジェクトのエフェクトモードをCLOSEに設定
    for (int32_t i = 0; i < level; ++i) {

        if (i >= static_cast<int32_t>(ComboLevelObjType::COUNT)) {
            continue;
        }

        objects_[i]->SetEffectMode(ObjEffectMode::CLOSE);
    }
}

bool ComboLevelObjHolder::GetIsEasingFinish(const ComboLevelObjType& type, const int32_t& groupNum) const {
    if (objects_[ToIndex(type)]) {
        return objects_[ToIndex(type)]->GetIsEasingFinish(groupNum);
    }
    return true;
}

void ComboLevelObjHolder::SetEffectMode(const ComboLevelObjType& type, const ObjEffectMode& mode) {

    if (objects_[ToIndex(type)]) {
        objects_[ToIndex(type)]->SetEffectMode(mode);
    }
}