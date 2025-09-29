#pragma once
#include "../BaseCSPipeline.h"
#include <Windows.h>

class SkinningCSPipeline : public BaseCSPipeline {
public:
    SkinningCSPipeline()          = default;
    virtual ~SkinningCSPipeline() = default;

    // BaseCSPipelineから継承
    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) override;

protected:
    void CreateRootSignature() override;
    void CreateComputePipeline() override;

private:
 
};