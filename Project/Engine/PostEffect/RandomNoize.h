#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>

/// <summary>
/// ランダムノイズポストエフェクトクラス
/// </summary>
class RandomNoize : public BasePostEffect {
public:
    struct RandomParamData {
        float time;
    };

private:
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成
    void CreateRootSignature() override; //< ルートシグネチャ作成

public:
    RandomNoize()           = default;
    ~RandomNoize() override = default;

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
    RandomParamData* paramData_;
};