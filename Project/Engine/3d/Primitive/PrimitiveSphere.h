#pragma once

#include "3D/Primitive/IPrimitive.h"

namespace KetaEngine {

/// <summary>
/// 球体プリミティブクラス
/// </summary>
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