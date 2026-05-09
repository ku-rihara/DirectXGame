#pragma once

#include "3D/Primitive/IPrimitive.h"

/// <summary>
/// リングプリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitiveRing : public IPrimitive {
public:
    struct RingParams {
        float   outerRadius = 1.0f;  ///< 外径
        float   innerRadius = 0.2f;  ///< 内径
        int32_t divisions   = 32;    ///< 分割数
    };

public:
    PrimitiveRing()  = default;
    ~PrimitiveRing() = default;

    void Init() override;   //< 初期化
    void Create() override; //< リング生成

    /// パラメータを設定してメッシュを再構築する
    void SetParams(const RingParams& params) { params_ = params; }
    const RingParams& GetParams() const { return params_; }
    void Rebuild();

private:
    RingParams params_;
};

}; // KetaEngine
