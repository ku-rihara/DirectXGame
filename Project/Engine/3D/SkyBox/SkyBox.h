#pragma once
#include "3D/Primitive/PrimitiveBox.h"
#include "3D/ViewProjection.h"
#include "3D/WorldTransform.h"
#include "Base/Material/SkyBoxMaterial.h"
#include "GPUData/TransformationMatrix.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

namespace KetaEngine {

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
    void Draw(ViewProjection& viewProjection);

private:
    WorldTransform transform_;
    std::unique_ptr<PrimitiveBox> primitiveBox_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_ = nullptr;
    SkyBoxMaterial skyBoxMaterial_;
    uint32_t textureHandle_ = 0;
};

} // namespace KetaEngine
