#pragma once

#include "Base/Material/ModelMaterial.h"
#include <array>
#include <d3d12.h>
#include <functional>
#include <memory>
#include <unordered_map>

namespace KetaEngine {

class DirectXCommon;
class BaseCSPipeline;
class SkinningCSPipeline;

enum class CSPipelineType {
    Skinning,
    Particle_Init,
    Particle_Emit,
    Particle_Update,
    Count
};

/// <summary>
/// コンピュートシェーダパイプライン管理クラス
/// </summary>
class CSPipelineManager {
public:
    static CSPipelineManager* GetInstance();

    CSPipelineManager(const CSPipelineManager&)            = delete;
    CSPipelineManager& operator=(const CSPipelineManager&) = delete;
    CSPipelineManager(CSPipelineManager&&)                 = delete;
    CSPipelineManager& operator=(CSPipelineManager&&)      = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="type">パイプラインタイプ</param>
    /// <param name="commandList">コマンドリスト</param>
    void PreDraw(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList) const;

    /// <summary>
    /// ディスパッチ実行
    /// </summary>
    /// <param name="type">パイプラインタイプ</param>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="numThreadsX">スレッド数X</param>
    void DisPatch(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX);

private:
    CSPipelineManager()  = default;
    ~CSPipelineManager() = default;

private:
    std::array<std::unique_ptr<BaseCSPipeline>, static_cast<size_t>(CSPipelineType::Count)> pipelines_;
    DirectXCommon* dxCommon_ = nullptr;

public:
    BaseCSPipeline* GetPipeline(const CSPipelineType& type) const;
};

}; // KetaEngine
