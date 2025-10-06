#pragma once
#include "../BaseBackGroundObject.h"
#include <cstdint>
#include <string>

enum class ObjEffectMode {
    NONE,
    SPAWN,
    WAIT,
    CLOSE,
};

class BaseComboLevelBackObj : public BaseBackGroundObject {
public:
    BaseComboLevelBackObj()          = default;
    virtual ~BaseComboLevelBackObj() = default;

    virtual void Init(const std::string& filename) override;
    virtual void Update(const float& playSpeed) override;
    virtual void Draw(const ViewProjection& viewProjection) override;

private:
    ObjEffectMode effectMode_;

public:
    void SetEffectMode(const ObjEffectMode& mode) { effectMode_ = mode; }
    int32_t ConvertEffectModeToInt(const ObjEffectMode& mode);
};
