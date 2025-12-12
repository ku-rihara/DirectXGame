#pragma once

#include "Primitive/IPrimitive.h"
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

    /// <summary>
    /// テクスチャ設定
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    void SetTexture(const std::string& name) override;

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="worldTransform">ワールドトランスフォーム</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    /// <param name="textureHandle">テクスチャハンドル</param>
    void Draw(
        const WorldTransform& worldTransform,
        const ViewProjection& viewProjection,
        std::optional<uint32_t> textureHandle = std::nullopt) override;

private:
};

}; // KetaEngine
