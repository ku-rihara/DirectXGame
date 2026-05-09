#pragma once

#include "3D/Primitive/IPrimitive.h"
#include <utility>

/// <summary>
/// ボックスプリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitiveBox : public IPrimitive {
public:
    PrimitiveBox()  = default;
    ~PrimitiveBox() = default;

    void Init() override;   //< 初期化
    void Create() override; //< ボックス生成

private:
};

}; // KetaEngine
