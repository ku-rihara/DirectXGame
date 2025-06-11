#pragma once

#include <string>

class ComboUIController;
class BaseComboUIBehavior {
protected:
    // 振る舞い名
    std::string name_;
    // ボス
    ComboUIController* pUIController_ = nullptr;

public:
    BaseComboUIBehavior(const std::string& name, ComboUIController* UIController_);
    virtual ~BaseComboUIBehavior() {}

    virtual void Update() = 0;
    virtual void Debug() = 0;
};