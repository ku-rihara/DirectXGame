#pragma once

#include "BasePostEffect.h"
#include "Vector3.h"
#include <d3d12.h>
#include <string>
#include <wrl/client.h>

/// <summary>
/// ディゾルブエフェクトクラス
/// </summary>
class Dissolve : public BasePostEffect {
public:
    struct ParamData {
        float thresholdValue;
        Vector3 color;
    };

private:
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成
    void CreateRootSignature() override; //< ルートシグネチャ作成

public:
    Dissolve()           = default;
    ~Dissolve() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon) override;

    /// <summary>
    /// 描画ステート設定
    /// </summary>
    /// <param name="commandList">コマンドリスト
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override; //< 定数バッファ作成
    void DebugParamImGui() override; //< ImGuiデバッグパラメータ

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    ParamData* paramData_;
    std::string maskTextureName_;
    uint32_t textureIndex_;
};