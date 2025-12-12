#pragma once

#include <array>
#include <cstdint>
#include <d3d12.h>
#include <Matrix4x4.h>
#include <span>
#include <vector>
#include <wrl.h>

const uint32_t kNumMaxInfluence = 4;

namespace KetaEngine {

struct VertexInfluence {
    std::array<float, kNumMaxInfluence> weights;
    std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
    Matrix4x4 skeletonSpaceMatrix;
    Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinCluster {

    std::vector<Matrix4x4> inverseBindPoseMatrices;
    Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
    std::span<VertexInfluence> mappedInfluence;
    Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
    std::span<WellForGPU> mappedPalette;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;

    Microsoft::WRL::ComPtr<ID3D12Resource> outputVertexResource;

   D3D12_VERTEX_BUFFER_VIEW outputVertexBufferView;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView;

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> inputVertexSrvHandle;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> outputVertexUavHandle;
    Microsoft::WRL::ComPtr<ID3D12Resource> skinningInfoResource;
};

}; // KetaEngine
