#pragma once

#include "3D/Primitive/IPrimitive.h"

namespace KetaEngine {

/// <summary>
/// リングプリミティブクラス
/// </summary>
class PrimitiveRing : public IPrimitive {
public:
    /// <summary>
    /// リング形状の生成パラメータ(外径・内径・分割数)
    /// </summary>
    struct RingParams {
        float   outerRadius = 1.0f;  ///< 外径
        float   innerRadius = 0.2f;  ///< 内径
        int32_t divisions   = 32;    ///< 分割数
    };

public:
    PrimitiveRing()  = default;
    ~PrimitiveRing() = default;

    void Init() override;
    void Create() override;
    PrimitiveType GetPrimitiveType() const override { return PrimitiveType::Ring; }

    /// パラメータを設定してメッシュを再構築する
    void SetParams(const RingParams& params) { params_ = params; }
    const RingParams& GetParams() const { return params_; }
    void Rebuild();

private:
    RingParams params_;
};

}; // KetaEngine
