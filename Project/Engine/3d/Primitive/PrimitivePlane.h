#pragma once

#include "3D/Primitive/IPrimitive.h"

/// <summary>
/// 平面プリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitivePlane : public IPrimitive {
public:
    PrimitivePlane()  = default;
    ~PrimitivePlane() = default;

    void Init() override;
    void Create() override;
    PrimitiveType GetPrimitiveType() const override { return PrimitiveType::Plane; }
  
private:
};

}; // KetaEngine
