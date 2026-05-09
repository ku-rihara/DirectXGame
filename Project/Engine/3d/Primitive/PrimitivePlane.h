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

    void Init() override;   //< 初期化
    void Create() override; //< 平面生成
  
private:
};

}; // KetaEngine
