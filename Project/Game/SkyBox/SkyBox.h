#pragma once
#include "3d/WorldTransform.h"
#include "3d/ViewProjection.h"
#include "3D/Primitive/PrimitiveBox.h"
#include "Base/Material/SkyBoxMaterial.h"
#include "struct/TransformationMatrix.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

/// <summary>
/// スカイボックス管理クラス
/// </summary>
class SkyBox {
public:
    SkyBox();
    ~SkyBox();

    void Init();
    void Update();
    void Debug();

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Draw(KetaEngine::ViewProjection& viewProjection);

private:
    KetaEngine::WorldTransform transform_;
    std::unique_ptr<KetaEngine::PrimitiveBox> primitiveBox_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    KetaEngine::TransformationMatrix* wvpDate_ = nullptr;
    KetaEngine::SkyBoxMaterial skyBoxMaterial_;
    uint32_t textureHandle_ = 0;
};
