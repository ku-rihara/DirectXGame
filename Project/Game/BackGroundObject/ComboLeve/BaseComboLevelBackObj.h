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

    void EasingResetSelectGroup(const int32_t& groupNum);
    void EasingAllReset();
    bool GetIsEasingFinish(const int32_t& groupNum) const;

protected:
    ObjEffectMode effectMode_ = ObjEffectMode::NONE;
    bool isPulseCycleStart_   = false;

public:
    void SetEffectMode(const ObjEffectMode& mode) { effectMode_ = mode; }
    int32_t ConvertEffectModeToInt(const ObjEffectMode& mode);
    void SetIsPulseCycleStart(const bool& isStart) { isPulseCycleStart_ = isStart; }
};
