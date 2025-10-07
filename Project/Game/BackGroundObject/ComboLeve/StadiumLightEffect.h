#pragma once
#include "BaseComboLevelBackObj.h"

class StadiumLightEffect : public BaseComboLevelBackObj {
public:
    StadiumLightEffect()          = default;
    virtual ~StadiumLightEffect() = default;

    void Init(const std::string& filename) override;
    void Update(const float& playSpeed) override;
  
private:
};