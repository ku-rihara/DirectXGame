#pragma once
#include "BaseComboLevelBackObj.h"

class SpeakerEffect : public BaseComboLevelBackObj {
public:
    SpeakerEffect()          = default;
     ~SpeakerEffect()override = default;

    void Init(const std::string& filename) override;
    void Update(const float& playSpeed) override;


public:
   
};