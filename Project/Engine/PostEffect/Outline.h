#pragma once

#include "BasePostEffect.h"
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>

/// <summary>
/// アウトラインポストエフェクトクラス
/// </summary>
class Outline : public BasePostEffect {
public:
    struct ParamData {
        float wightRate;
    };

    struct OutLineMaterial {
        Matrix4x4 projectionInverse;
    };

private:
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成
    void CreateRootSignature() override; //< ルートシグネチャ作成

public:
    Outline()           = default;
    ~Outline() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon) override;

    /// <summary>
    /// 描画ステート設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override; //< 定数バッファ作成
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override; //< 描画処理
    void DebugParamImGui() override; //< ImGuiデバッグパラメータ

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> outlineMaterialResource_;
    ParamData* paramData_;
    OutLineMaterial* outlineMaterialData_;

    D3D12_STATIC_SAMPLER_DESC staticSamplersOutLine_[2];
};