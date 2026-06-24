#pragma once

#include "3D/Primitive/IPrimitive.h"

/// <summary>
/// ボックスプリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitiveBox : public IPrimitive {
public:
    PrimitiveBox()  = default;
    ~PrimitiveBox() = default;

    void Init() override;
    void Create() override;
    PrimitiveType GetPrimitiveType() const override { return PrimitiveType::Box; }

private:
};

}; // KetaEngine
