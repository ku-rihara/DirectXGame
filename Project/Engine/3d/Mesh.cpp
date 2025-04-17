#include "Mesh.h"
#include "base/DirectXCommon.h"
#include "base/TextureManager.h"
#include "Lighrt/Light.h"

void Mesh::Init(DirectXCommon* directXCommon, const uint32_t& vertexNum) {
    directXCommon_ = directXCommon;
    vertexNum_     = vertexNum;

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

    defaultTextureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/Texture/default.png");
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
        commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(defaultTextureHandle_));
    }

    commandList->DrawInstanced(UINT(vertexNum_), instanceNum, 0, 0);
    
}
