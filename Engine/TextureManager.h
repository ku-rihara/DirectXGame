#pragma once
#include<wrl.h>
#include <string>
#include<d3d12.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<vector>

class TextureManager {
private:
	DirectX::ScratchImage image_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource> textureResource_;
	DirectX::ScratchImage mipImages_;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandles_;
	
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

	/* void Load();*/

	 uint32_t LoadTextureResource(const std::string& filePath);


	 D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;

	//getter
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }
	ID3D12Resource* GetTextureResource()const { return textureResource_.Get(); }
	const DirectX::ScratchImage& GetMipImages() { return mipImages_; }

	//D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2()const { return textureSrvHandleGPU2_; }
	//ID3D12Resource* GetTextureResource2()const { return textureResource2_.Get(); }
	//const DirectX::ScratchImage& GetMipImages2() { return mipImages2_; }

	//D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU3()const { return textureSrvHandleGPU3_; }
	//ID3D12Resource* GetTextureResource3()const { return textureResource3_.Get(); }
	//const DirectX::ScratchImage& GetMipImages3() { return mipImages3_; }

	//D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU4()const { return textureSrvHandleGPU4_; }
	//ID3D12Resource* GetTextureResource4()const { return textureResource4_.Get(); }
	//const DirectX::ScratchImage& GetMipImages4() { return mipImages4_; }
};
