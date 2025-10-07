#pragma once
#include "../BaseBackGroundObject.h"
#include <cstdint>
#include <string>

enum class ObjEffectMode {
    SPAWN,
    PULSE,
    CLOSE,
    NONE,
};

class BaseComboLevelBackObj : public BaseBackGroundObject {
public:
    BaseComboLevelBackObj()          = default;
    virtual ~BaseComboLevelBackObj() = default;

    virtual void Init(const std::string& filename) override;
    virtual void Update(const float& playSpeed) override;
    virtual void Draw(const ViewProjection& viewProjection) override;

    void EasingResetSelectGroup(const int32_t& groupNum);
    void EasingAllReset();
    bool GetIsEasingFinish(const int32_t& groupNum)const;

private:
    ObjEffectMode effectMode_=ObjEffectMode::NONE;

public:
    void SetEffectMode(const ObjEffectMode& mode) { effectMode_ = mode; }
    int32_t ConvertEffectModeToInt(const ObjEffectMode& mode);
};
