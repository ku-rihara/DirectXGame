#include "ComboLevelObjHolder.h"

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
    // 新しいオブジェクトタイプはここに追加
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