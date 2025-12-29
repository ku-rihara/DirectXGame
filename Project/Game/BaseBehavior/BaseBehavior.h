#pragma once

#include <string>

template <typename OwnerType>
class BaseBehavior {
public:
    BaseBehavior(const std::string name, OwnerType* owner)
        : name_(name), pOwner_(owner) {}

    virtual ~BaseBehavior() = default;

    virtual void Update(float timeSpeed) = 0;
    virtual void Debug()                 = 0;

protected:
    std::string name_;
    OwnerType* pOwner_;

public:
    const std::string& GetName() const { return name_; }
    OwnerType* GetOwner() const { return pOwner_; }
};