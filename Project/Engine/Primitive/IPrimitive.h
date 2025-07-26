#pragma once
#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <wrl.h>

// transform 
#include "struct/TransformationMatrix.h"
#include"3d/WorldTransform.h"
#include "3d/Mesh.h"

#include <optional>
#include<string>

enum class PrimitiveType {
    Plane,
    Sphere,
    Cylinder,
    Ring,
    Box
};

class IPrimitive {
public:
    IPrimitive()          = default;
    virtual ~IPrimitive() = default;

    virtual void Init();
    virtual void Create() = 0;
    virtual void Draw(
        const WorldTransform& worldTransform, 
        const ViewProjection& viewProjection, 
        std::optional<uint32_t> textureHandle=std::nullopt
    );

    virtual void SetTexture(const std::string& name);

      void CreateWVPResource();
     void CreateMaterialResource();

protected:
    std::unique_ptr<Mesh> mesh_ = nullptr;
    uint32_t vertexNum_;
    uint32_t indexNum_;
    MeshMaterial material_;

     Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_;

public:
   
    ///==========================================================
    /// getter method
    ///==========================================================
    Mesh* GetMesh() const { return mesh_.get(); }
    ///==========================================================
    /// setter method
    ///==========================================================
    void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
    void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
};