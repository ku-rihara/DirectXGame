#include "ComboLevelObjHolder.h"

void ComboLevelObjHolder::Init() {
   
}

void ComboLevelObjHolder::Update(const float& playSpeed) {
    // 各オブジェクトの更新
    for (auto& obj : objects_) {
        if (obj) {
            obj->Update(playSpeed);
        }
    }
}

void ComboLevelObjHolder::Draw(const ViewProjection& viewProjection) {
    // 各オブジェクトの描画
    for (auto& obj : objects_) {
        if (obj) {
            obj->Draw(viewProjection);
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
  
    default:
        break;
    }

    // 生成したオブジェクトを初期化
    if (objects_[index]) {
        objects_[index]->Init(filename);
    }
}

void ComboLevelObjHolder::SetEffectMode(const ComboLevelObjType& type, const ObjEffectMode& mode) {
    if (objects_[ToIndex(type)]) {
        objects_[ToIndex(type)]->SetEffectMode(mode);
    }
}