#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "Vector3.h"

struct ComboAttackParameter {
    std::string name;
    std::string startCondition;
    std::string endCondition;
    std::string motionType;
    std::string easingPreset;
    float easingTime = 0.3f;
    Vector3 collisionSize = {1,1,1};
    Vector3 collisionOffset = {0,0,0};
    std::string collisionType;
  
};

class ComboCreateEditor {
public:
    void Edit();
    void Load(const std::string& path);
    void Save(const std::string& path);

    std::vector<ComboAttackParameter> combos_;
    int selectedCombo_ = 0;
};