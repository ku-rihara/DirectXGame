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

    void Init() override; //< 初期化
    void Create() override; //< 球体生成

private:
};

} // KetaEngine