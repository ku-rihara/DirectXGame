#pragma once
#include "BaseComboLevelBackObj.h"

class StadiumLightEffect : public BaseComboLevelBackObj {
public:
    StadiumLightEffect()  = default;
    ~StadiumLightEffect()override = default;

    void Init(const std::string& filename) override;
    void Update(const float& playSpeed) override;

private:
    bool isPulseOneCycleEnd_ = false;

public:
    const bool& GetIsPulseOneCycleEnd() const { return isPulseOneCycleEnd_; }
    void SetIsPulseOneCycleEnd(const bool& isEnd) { isPulseOneCycleEnd_ = isEnd; }
};