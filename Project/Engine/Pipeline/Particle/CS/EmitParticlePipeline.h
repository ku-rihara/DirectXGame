#pragma once
#include "../Engine/Pipeline/BaseCSPipeline.h"
#include <Windows.h>

/// <summary>
/// パーティクル生成用コンピュートシェーダパイプラインクラス
/// </summary>
namespace KetaEngine {

class EmitParticlePipeline : public BaseCSPipeline {
public:
    EmitParticlePipeline()          = default;
    virtual ~EmitParticlePipeline() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon) override;

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;

    /// <summary>
    /// ディスパッチ実行
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="numVertices">頂点数</param>
    void Dispatch(ID3D12GraphicsCommandList* commandList, const UINT& numVertices) override;

protected:
    void CreateRootSignature() override; //< ルートシグネチャ作成
    void CreateComputePipeline() override; //< コンピュートパイプライン作成

private:
};

}; // KetaEngine
