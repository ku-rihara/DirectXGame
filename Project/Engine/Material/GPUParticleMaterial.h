#pragma once
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace KetaEngine {

class DirectXCommon;

class GPUParticleMaterial {
private:
    struct MaterialData {
        Vector4 color;

        float shininess;
        float padding0[3];

        Matrix4x4 uvMatrix;

        int32_t enableLighting;
        float environmentCoefficient;
        float padding1[2];
    };

    struct UVAnimationData {
        Vector2 uvPosition;
        float uvRotate;
        float padding0;

        Vector2 uvScrollSpeed;
        Vector2 uvScale = Vector2::OneVector();

        int32_t isFlipX;
        int32_t isFlipY;
        float pixelStep;
        int32_t isUVLoop;

        float currentTime;
        float padding1[3];
    };

public:
    GPUParticleMaterial()  = default;
    ~GPUParticleMaterial() = default;

    void CreateMaterialResource(DirectXCommon* dxCommon);
    void UpdateMaterialData(const Vector4& Color);
    void SetCommandList(ID3D12GraphicsCommandList* commandList);
    void DebugImGui();

    // UV関連
    void UpdateUVAnimation(float deltaTime);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> uvAnimeResource_;

    MaterialData* materialData_   = nullptr;
    UVAnimationData* uvAnimeData_ = nullptr;

public:
    MaterialData* GetMaterialData() { return materialData_; }
    UVAnimationData* GetUVAnimeData() { return uvAnimeData_; }
    ID3D12Resource* GetUVAnimeResource() { return uvAnimeResource_.Get(); }
};

}; // KetaEngine
