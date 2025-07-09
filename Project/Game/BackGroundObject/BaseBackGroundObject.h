#pragma once
#include "3d/ViewProjection.h"
#include "utility/PutObjForBlender/PutObjForBlender.h"
#include <memory>
#include <string>

class BaseBackGroundObject {
public:
    BaseBackGroundObject()          = default;
    virtual ~BaseBackGroundObject() = default;

    virtual void Init(const std::string& filename);
    virtual void Update()                                   = 0;
    virtual void Draw(const ViewProjection& viewProjection) = 0;

protected:
    std::unique_ptr<PutObjForBlender> putObjForBlender_;
};