#pragma once

#include "BasePostEffect.h"
#include <d3d12.h>
#include "Vector2.h"
#include <wrl/client.h>

class RadialBlur : public BasePostEffect {
public:
    struct ParamData {
        Vector2 center;
        float blurWidth;
    };

private:
    void CreateGraphicsPipeline() override;
    void CreateRootSignature() override;

public:
    RadialBlur()           = default;
    ~RadialBlur() override = default;

    void Init(DirectXCommon* dxCommon) override;
    void SetDrawState(ID3D12GraphicsCommandList* commandList) override;

    void CreateConstantBuffer() override;
    void Draw([[maybe_unused]] ID3D12GraphicsCommandList* commandList) override;
    void DebugParamImGui() override;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> paramDataResource_;
    ParamData* paramData_;

public:
   
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
