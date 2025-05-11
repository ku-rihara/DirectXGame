#pragma once

#include"Primitive/IPrimitive.h"

class PrimitiveCylinder:
	public IPrimitive{
public:

     PrimitiveCylinder() = default;
    ~PrimitiveCylinder() = default;

	void Init() override;
    void Create() override;
    void SetTexture(const std::string& name) override;
    void Draw(
        const WorldTransform& worldTransform,
        const ViewProjection& viewProjection,
        std::optional<uint32_t> textureHandle = std::nullopt) override;
	
private:



};

