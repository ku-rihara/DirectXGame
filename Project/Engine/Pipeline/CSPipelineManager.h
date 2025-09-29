#pragma once

#include <array>
#include <d3d12.h>
#include <functional>
#include <Material/ModelMaterial.h>
#include <memory>
#include <unordered_map>

class DirectXCommon;
class BaseCSPipeline;
class SkinningCSPipeline;

enum class CSPipelineType {
    Skinning,
    Count
};

class CSPipelineManager {
public:
    static CSPipelineManager* GetInstance();

    CSPipelineManager(const CSPipelineManager&)            = delete;
    CSPipelineManager& operator=(const CSPipelineManager&) = delete;
    CSPipelineManager(CSPipelineManager&&)                 = delete;
    CSPipelineManager& operator=(CSPipelineManager&&)      = delete;

    void Init(DirectXCommon* dxCommon);
    void PreDraw(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList) const;
    void DisPatch(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX);

    private : CSPipelineManager() = default;
    ~CSPipelineManager()          = default;

private:
    std::array<std::unique_ptr<BaseCSPipeline>, static_cast<size_t>(CSPipelineType::Count)> pipelines_;
    DirectXCommon* dxCommon_ = nullptr;

public:
    // getter
    BaseCSPipeline* GetPipeline(const CSPipelineType& type) const;
};