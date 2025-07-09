#pragma once
#include "3d/ViewProjection.h"
#include "utility/PutObjForBlender/PutObjForBlender.h"
#include <memory>

class BaseBackGroundObject {
public:
    BaseBackGroundObject()          = default;
    virtual ~BaseBackGroundObject() = default;

    virtual void Initialize()                               = 0;
    virtual void Update(float deltaTime)                    = 0;
    virtual void Draw(const ViewProjection& viewProjection) = 0;

private:
    std::unique_ptr<PutObjForBlender>putObjForBlender_;
};