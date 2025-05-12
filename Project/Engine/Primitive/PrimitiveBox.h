#pragma once

#include "Primitive/IPrimitive.h"
#include <utility>

class PrimitiveBox : public IPrimitive {
public:

    PrimitiveBox()  = default;
    ~PrimitiveBox() = default;

    void Init() override;
    void Create() override;
    void SetTexture(const std::string& name) override;
    void Draw(
        const WorldTransform& worldTransform,
        const ViewProjection& viewProjection,
        std::optional<uint32_t> textureHandle = std::nullopt) override;

private:
 
};
