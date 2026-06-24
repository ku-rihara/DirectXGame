#pragma once

#include "3D/Primitive/IPrimitive.h"

/// <summary>
/// 球体プリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitiveSphere : public IPrimitive {
public:
    PrimitiveSphere()  = default;
    ~PrimitiveSphere() = default;

    void Init() override;
    void Create() override;
    PrimitiveType GetPrimitiveType() const override { return PrimitiveType::Sphere; }

private:
};

} // KetaEngine