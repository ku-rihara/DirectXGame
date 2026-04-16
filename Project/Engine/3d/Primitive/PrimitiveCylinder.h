#pragma once

#include "3D/Primitive/IPrimitive.h"
#include "Vector4.h"
#include <cstdint>

/// <summary>
/// シリンダープリミティブクラス
/// </summary>
namespace KetaEngine {

class PrimitiveCylinder : public IPrimitive {
public:
    /// <summary>
    /// シリンダー形状パラメータ
    /// </summary>
    struct CylinderParams {
        float   topRadiusX      = 1.0f;  ///< 上半径X
        float   topRadiusZ      = 1.0f;  ///< 上半径Z (楕円時)
        float   bottomRadiusX   = 1.0f;  ///< 下半径X
        float   bottomRadiusZ   = 1.0f;  ///< 下半径Z (楕円時)
        float   height          = 3.0f;  ///< 高さ
        int32_t divisions       = 32;    ///< 周方向分割数
        int32_t heightDivisions = 1;     ///< 縦方向分割数
        float   startAngleDeg   = 0.0f;  ///< 開始角度 (度)
        float   endAngleDeg     = 360.0f;///< 終了角度 (度)
        int32_t uvModeInt       = 0;     ///< 0=上→下 1=下→上 2=横方向
        int32_t axisInt         = 0;     ///< 回転軸 0=Y軸 1=X軸 2=Z軸
        Vector4 topColor    = Vector4(1.0f, 1.0f, 1.0f, 1.0f); ///< 上端頂点カラー
        Vector4 bottomColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f); ///< 下端頂点カラー
    };

public:
    PrimitiveCylinder()  = default;
    ~PrimitiveCylinder() = default;

    void Init() override;   //< 初期化
    void Create() override; //< シリンダー生成

    void SetTexture(const std::string& name) override;
    void Draw(
        const WorldTransform& worldTransform,
        const ViewProjection& viewProjection,
        std::optional<uint32_t> textureHandle = std::nullopt) override;

    /// パラメータを設定してメッシュを再構築する
    void SetParams(const CylinderParams& params) { params_ = params; }
    const CylinderParams& GetParams() const { return params_; }
    void Rebuild();

private:
    static Vector4 LerpColor(const Vector4& a, const Vector4& b, float t);
    Vector2        GetUV(float u, float v) const;
    Vector4        ApplyAxis(float circX, float circY, float h) const;

    const int kVerticesPerFace = 6;

    CylinderParams params_;
};

}; // KetaEngine
