#include "Material.h"
#include <cassert>
#include"DirectXCommon.h"
// 初期化
void Material::Init() {
    CreateConstBuffer();
}

// 定数バッファを生成する
void Material::CreateConstBuffer() {
    // 定数バッファのサイズは適切なアラインメントを考慮
    const UINT64 bufferSize = sizeof(ConstBufferDataMaterial);

    // リソースのヒーププロパティ
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    // リソースのディスクリプション
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.Width = bufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // 定数バッファを作成
    HRESULT result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constBuffer_)
    );
    assert(SUCCEEDED(result));
    Map();
}

// マッピングする
void Material::Map() {
    // マッピングしてポインタを取得
    HRESULT result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
    assert(SUCCEEDED(result));
    // 初期化時にデータを転送
    TransferData();
}

// マテリアルデータを転送する
void Material::TransferData() {
    if (constMap_) {
        constMap_->color_ = color_;
        constMap_->enableLighting = enableLighting_;
        constMap_->uvTransform = uvTransform_;
        constMap_->shininess = shininess_;
    }
}

// グラフィックスコマンドを積む
void Material::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) const {
    commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuffer_->GetGPUVirtualAddress());
}
