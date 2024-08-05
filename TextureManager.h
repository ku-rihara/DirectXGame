#pragma once
#include<wrl.h>
#include <string>
#include<d3d12.h>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include<vector>

class TextureManager {
private:
	DirectX::ScratchImage image_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource_;
	DirectX::ScratchImage mipImages_;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandles_;
	//2
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2_;
	DirectX::ScratchImage mipImages2_;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource2_;
	//3
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU3_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU3_;
	DirectX::ScratchImage mipImages3_;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource3_;
public:
	
	static uint32_t descriptorHeapIndex_;

	static TextureManager* GetInstance();
	//TextureデータをCPUで読み込む
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	//Resourceデータの作成
	Microsoft::WRL::ComPtr < ID3D12Resource>  CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	//データを転送する関数
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureDate(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device>device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	/* uint32_t Load(const std::string& fileName);*/

	 void Load();

	 D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;

	//getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }
	ID3D12Resource* GetTextureResource()const { return textureResource_.Get(); }
	const DirectX::ScratchImage& GetMipImages() { return mipImages_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2()const { return textureSrvHandleGPU2_; }
	ID3D12Resource* GetTextureResource2()const { return textureResource2_.Get(); }
	const DirectX::ScratchImage& GetMipImages2() { return mipImages2_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU3()const { return textureSrvHandleGPU3_; }
	ID3D12Resource* GetTextureResource3()const { return textureResource3_.Get(); }
	const DirectX::ScratchImage& GetMipImages3() { return mipImages3_; }
};

