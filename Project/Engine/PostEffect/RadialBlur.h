#pragma once

#include "BasePostEffect.h"
#include "Vector2.h"
#include <d3d12.h>
#include <wrl/client.h>

/// <summary>
/// ラジアルブラーポストエフェクトクラス
/// </summary>
namespace KetaEngine {

class RadialBlur : public BasePostEffect {
public:
    struct ParamData {
        Vector2 center;
        float blurWidth;
    };

private:
    void CreateGraphicsPipeline() override; //< グラフィックスパイプライン作成
    void CreateRootSignature() override; //< ルートシグネチャ作成

public:
    RadialBlur()           = default;
    ~RadialBlur() override = default;

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
    ParamData* paramData_;

public:

    // setter
    void SetCenter(const Vector2& center) {
        if (paramData_) {
            paramData_->center = center;
        }
    }

    void SetBlurWidth(float blurWidth) {
        if (paramData_) {
            paramData_->blurWidth = blurWidth;
        }
    }
};

}; // KetaEngine
