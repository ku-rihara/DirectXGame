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

private:
   /* bool isDraw_;*/

public:
    // Getter/Setter
   /* bool GetIsDraw() const { return isDraw_; }
    void SetIsDraw(bool is) { isDraw_ = is; }*/
};