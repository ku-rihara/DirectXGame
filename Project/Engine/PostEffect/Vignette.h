#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>
#include <wrl/client.h>

/// <summary>
/// ビネットポストエフェクトクラス
/// </summary>
namespace KetaEngine {

class Vignette : public BasePostEffect {
public:
    struct ParamData {
        float redIntensity = 0.0f;
    };

private:
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成
    void CreateRootSignature() override; //< ルートシグネチャ作成

public:
    Vignette()           = default;
    ~Vignette() override = default;

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
    ParamData* paramData_ = nullptr;

public:
    void SetRedIntensity(float intensity) {
        if (paramData_) {
            paramData_->redIntensity = intensity;
        }
    }
};

}; // KetaEngine
