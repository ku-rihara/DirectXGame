#pragma once
#include "../Engine/Pipeline/BaseCSPipeline.h"
#include <Windows.h>

class InitParticlePipeline : public BaseCSPipeline {
public:
    InitParticlePipeline()          = default;
    virtual ~InitParticlePipeline() = default;

    // BaseCSPipelineから継承
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) override;

protected:
    void CreateRootSignature() override;
    void CreateComputePipeline() override;

private:
};