#include "Mesh.h"
#include "base/DirectXCommon.h"
#include "base/TextureManager.h"

void Mesh::Init(DirectXCommon* directXCommon, const uint32_t& vertexNum) {
    directXCommon_ = directXCommon;
    vertexNum_     = vertexNum;

    // Resource作成
    CreateVertexResource();
   
    //デフォルトテクスチャ設定
    textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/Texture/default.png");
}

void Mesh::SetIndexData(const uint32_t* indices, uint32_t indexCount) {
    indexNum_ = indexCount;

    // インデックスバッファを作成
    indexResource_ = directXCommon_->CreateBufferResource(
        directXCommon_->GetDevice(),
        sizeof(uint32_t) * indexCount);

    // インデックスバッファにデータを書き込む
    uint32_t* mappedIndexData = nullptr;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData));
    memcpy(mappedIndexData, indices, sizeof(uint32_t) * indexCount);
    indexResource_->Unmap(0, nullptr);

    // インデックスバッファビューを設定
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes    = sizeof(uint32_t) * indexCount;
    indexBufferView_.Format         = DXGI_FORMAT_R32_UINT;
}

void Mesh::SetTexture(const std::string& name) {
    textureHandle_ = TextureManager::GetInstance()->LoadTexture(name);
}

void Mesh::CreateVertexResource() {
    // VeexBufferViewを作成する
    vertexResource_ = directXCommon_->CreateBufferResource(directXCommon_->GetDevice(), sizeof(VertexData) * vertexNum_);
    // 頂点バッファビューを作成する
    vertexBufferView_ = {};
    // リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    // 使用するリソースのサイズは頂点3つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexNum_;
    // 頂点当たりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    // 書き込むためのアドレスを取得
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate_));
}


void Mesh::Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource,Material material, std::optional<uint32_t> textureHandle) {

    auto commandList = directXCommon_->GetCommandList();
    /*materialDate_->color = color.;*/

    // 頂点バッファとインデックスバッファの設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    // commandList->IASetIndexBuffer(&indexBufferView_);  // IBV

    // 形状を設定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    material.SetCommandList(commandList);

    commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

    if (textureHandle.has_value()) {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
    } else {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    }
  /*  Light::GetInstance()->SetLightCommands(commandList);*/

    // 描画コール
    commandList->DrawInstanced(vertexNum_, 1, 0, 0);
}

void Mesh::DrawInstancing(const uint32_t instanceNum, D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle, Material material,
    std::optional<uint32_t> textureHandle) {

    auto commandList = directXCommon_->GetCommandList();

    // ルートシグネチャとパイプラインステートを設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetIndexBuffer(&indexBufferView_);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアルのリソースを設定
    material.SetCommandList(commandList);
    commandList->SetGraphicsRootDescriptorTable(1, instancingGUPHandle);

    // テクスチャハンドルの設定
    if (textureHandle.has_value()) {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle.value()));
    } else {
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    }

    commandList->DrawInstanced(vertexNum_, instanceNum, 0, 0);
}