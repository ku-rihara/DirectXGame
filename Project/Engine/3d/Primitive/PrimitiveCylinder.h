#pragma once

#include "3D/Primitive/IPrimitive.h"
#include "Editor/Easing/Easing.h"
#include "Vector4.h"
#include <cstdint>
#include <memory>

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
        float topRadiusX        = 1.0f; ///< 上半径X
        float topRadiusZ        = 1.0f; ///< 上半径Z
        float bottomRadiusX     = 1.0f; ///< 下半径X
        float bottomRadiusZ     = 1.0f; ///< 下半径Z
        float height            = 3.0f; ///< 高さ
        int32_t divisions       = 32; ///< 周方向分割数
        int32_t heightDivisions = 1; ///< 縦方向分割数
        float startAngleDeg     = 0.0f; ///< 開始角度 (度)
        float endAngleDeg       = 360.0f; ///< 終了角度 (度)
        int32_t uvModeInt       = 0; ///< 0=上→下 1=下→上 2=横方向
        int32_t axisInt         = 0; ///< 回転軸 0=Y軸 1=X軸 2=Z軸
        Vector4 topColor        = Vector4(1.0f, 1.0f, 1.0f, 1.0f); ///< 上端頂点カラー
        Vector4 bottomColor     = Vector4(1.0f, 1.0f, 1.0f, 1.0f); ///< 下端頂点カラー
    };

public:
    PrimitiveCylinder()  = default;
    ~PrimitiveCylinder() = default;

    // 初期化、プリミティブ作成
    void Init() override;
    void Create() override;

    // プリミティブの再構築
    void Rebuild();

    /// 終点角度をEasing<float>でアニメーション
    /// param.startValue / endValue は内部でratio変換して上書きする
    void StartEndAngleEasing(float targetEndDeg, const EasingParameter<float>& easingParam);

    /// イージング更新
    void UpdateEasing(float deltaTime);

    bool IsEasingFinished() const;

private:
    Vector2 GetUV(float u, float v) const;
    Vector4 ApplyAxis(float circX, float circY, float h) const;

private:
    const int kVerticesPerFace = 6;
    CylinderParams params_;
    std::unique_ptr<Easing<float>> endAngleEasing_;

public:
    // Getter
    const CylinderParams& GetParams() const { return params_; }

    // Setter
    void SetParams(const CylinderParams& params) { params_ = params; }
    void SetEndAngleMask(float endDeg);
};

}; // KetaEngine
