#pragma once
#include "../Engine/Pipeline/BaseCSPipeline.h"
#include <Windows.h>

class EmitParticlePipeline : public BaseCSPipeline {
public:
    EmitParticlePipeline()        = default;
    virtual ~EmitParticlePipeline() = default;

    // BaseCSPipelineから継承
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) override;

protected:
    void CreateRootSignature() override;
    void CreateComputePipeline() override;

private:
};