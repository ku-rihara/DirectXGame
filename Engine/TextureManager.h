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
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResources_;
	std::vector<DirectX::ScratchImage> mipImages_;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandles_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;
	
public:
	
	static uint32_t descriptorHeapIndex_;

	static TextureManager* GetInstance();

	void Init(DirectXCommon* dxCommon);
	/// <summary>
	/// TextureデータをCPUで読み込む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	DirectX::ScratchImage LoadTextureFile(const std::string& filePath);
	/// <summary>
	/// Resourceデータの作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="metadata"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr <ID3D12Resource>  CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	/// <summary>
	/// データを転送する関数
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureDate(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device>device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <returns>テクスチャハンドル</returns>
	  uint32_t LoadTexture(const std::string& filePath);

	 D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;
private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

};

