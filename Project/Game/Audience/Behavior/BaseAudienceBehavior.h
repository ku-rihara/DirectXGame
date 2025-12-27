#pragma once

#include <string>

class Audience;

class BaseAudienceBehavior {
protected:
    // 振る舞い名
    std::string name_;
    // 観客
    Audience* pAudience_ = nullptr;
   
public:
    BaseAudienceBehavior(const std::string& name, Audience* audience);
    virtual ~BaseAudienceBehavior() {}

    virtual void Update() = 0;
    virtual void Debug()  = 0;

public:
    // Getter/Setter
};