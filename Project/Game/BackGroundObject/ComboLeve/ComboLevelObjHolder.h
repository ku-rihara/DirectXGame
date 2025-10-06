#pragma once
#include "BaseComboLevelBackObj.h"
#include "StadiumLightEffect.h"
#include <array>
#include <memory>
#include <string>

enum class ComboLevelObjType {
    STADIUM_LIGHT,
    COUNT
};

class ComboLevelObjHolder {
public:
    ComboLevelObjHolder()  = default;
    ~ComboLevelObjHolder() = default;

    void Init();
    void Update(const float& playSpeed);
    void Draw(const ViewProjection& viewProjection);

    size_t ToIndex(const ComboLevelObjType& type) const { return static_cast<size_t>(type); }

    // オブジェクトの追加
    void Add(const ComboLevelObjType& type, const std::string& filename);
  

private:
    std::array<std::unique_ptr<BaseComboLevelBackObj>, static_cast<size_t>(ComboLevelObjType::COUNT)> objects_;

public:
    void SetEffectMode(const ComboLevelObjType& type, const ObjEffectMode& mode);
};