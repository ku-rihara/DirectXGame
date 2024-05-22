#pragma once
#include <string>
#include<d3d12.h>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"


class TextureManager{
private:
    DirectX::ScratchImage image_{};
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
    ID3D12Resource* textureResource_;
    DirectX::ScratchImage mipImages_;
  //2
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2_;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2_;
    DirectX::ScratchImage mipImages2_;
    ID3D12Resource* textureResource2_;


public:

    static TextureManager* GetInstance();
    //TextureデータをCPUで読み込む
    DirectX::ScratchImage LoadTexture(const std::string& filePath);
    //Resourceデータの作成
    ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
    //データを転送する関数
    ID3D12Resource* UploadTextureDate(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    void Load();
    void ReleaseObject();

    //getter
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }
    ID3D12Resource* GetTextureResource()const { return textureResource_; }
    const DirectX::ScratchImage& GetMipImages() { return mipImages_;}

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2()const { return textureSrvHandleGPU2_; }
    ID3D12Resource* GetTextureResource2()const { return textureResource2_; }
    const DirectX::ScratchImage& GetMipImages2() { return mipImages2_; }
};

