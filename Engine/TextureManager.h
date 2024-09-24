#pragma once
#include<wrl.h>
#include <string>
#include<d3d12.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include"DirectXCommon.h"
#include<vector>

class TextureManager {
private:
	DirectX::ScratchImage image_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource_;
	DirectX::ScratchImage mipImages_;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandles_;
	// テクスチャデータのアップロード
	  Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResources_;
	
public:
	
	static uint32_t descriptorHeapIndex_;

	static TextureManager* GetInstance();

	void Init(DirectXCommon* dxCommon);
	//TextureデータをCPUで読み込む
	DirectX::ScratchImage LoadTextureFile(const std::string& filePath);
	//Resourceデータの作成
	Microsoft::WRL::ComPtr < ID3D12Resource>  CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	//データを転送する関数
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureDate(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device>device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	  uint32_t LoadTexture(const std::string& filePath);

	 D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;

	//getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }
	ID3D12Resource* GetTextureResource()const { return textureResource_.Get(); }
	const DirectX::ScratchImage& GetMipImages() { return mipImages_; }
};

