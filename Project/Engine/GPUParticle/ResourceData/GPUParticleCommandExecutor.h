#pragma once
#include <d3d12.h>

class GPUParticleResourceData;
class DirectXCommon;
class CSPipelineManager;
class SrvManager;


/// <summary>
/// GPUパーティクルのシェーダーバインドや、ディスパッチを行う
/// </summary>
class GPUParticleCommandExecutor {
public:
    GPUParticleCommandExecutor()  = default;
    ~GPUParticleCommandExecutor() = default;

    // 初期化
    void Init(GPUParticleResourceData* resourceData);

    /// <summary>
    /// GPUParticleの初期化パスを実行
    /// </summary>
    void ExecuteInitPass(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX);

    /// <summary>
    /// GPUParticleのエミットパスを実行
    /// </summary>
    void ExecuteEmitPass(ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX);

    /// <summary>
    /// GPUParticleの更新パスを実行
    /// </summary>
    void ExecuteUpdatePass(ID3D12GraphicsCommandList* commandLis, const UINT& numThreadsX);

  
private:
    GPUParticleResourceData* resourceData_;

    DirectXCommon* dxCommon_;
    CSPipelineManager* csPipeManager_;
    SrvManager* srvManager_;
};