#pragma once

#include"Vector3.h"
#include<vector>
#include <Matrix4x4.h>
#include <d3d12.h>
#include <span>
#include <cstdint>
#include<wrl.h>
#include <array>

const uint32_t kNumMaxInfluence = 4;

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

    // GPU用インフルエンスデータ
    Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
    D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
    std::span<VertexInfluence> mappedInfluence;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle;

    // GPU用パレットデータ
    Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
    std::span<WellForGPU> mappedPalette;
    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};