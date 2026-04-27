#pragma once
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <wrl.h>

#include "3d/Mesh.h"
#include "3d/WorldTransform.h"
#include "struct/TransformationMatrix.h"
#include "Base/Material/PrimitiveMaterial.h"

#include <optional>
#include <string>

enum class PrimitiveType {
    Plane,
    Sphere,
    Cylinder,
    Ring,
    Box
};

/// <summary>
/// プリミティブの基底クラス
/// </summary>
namespace KetaEngine {

class IPrimitive {
public:
    IPrimitive()          = default;
    virtual ~IPrimitive() = default;

    virtual void Init();       //< 初期化
    virtual void Create() = 0; //< プリミティブ生成

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="worldTransform">ワールドトランスフォーム</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    /// <param name="textureHandle">テクスチャハンドル</param>
    virtual void Draw(
        const WorldTransform& worldTransform,
        const ViewProjection& viewProjection,
        std::optional<uint32_t> textureHandle = std::nullopt);

    /// <summary>
    /// テクスチャ設定
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    virtual void SetTexture(const std::string& name);

    void CreateWVPResource(); //< WVPリソース作成
    void CreateMaterialResource(); //< マテリアルリソース作成

protected:
    void RebuildInternal(uint32_t newVertexNum); //< Rebuild 共通処理

    std::unique_ptr<Mesh> mesh_ = nullptr;
    std::string currentTexturePath_;

    uint32_t vertexNum_;
    uint32_t indexNum_;
    uint32_t primitiveTextureHandle_ = 0; 
    PrimitiveMaterial material_;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_;

public:
    Mesh* GetMesh() const { return mesh_.get(); }

    void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
    void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }

    void SetColor(const Vector4& color);
    PrimitiveMaterial& GetMaterial() { return material_; }
};

}; // KetaEngine
