#pragma once
#include<wrl.h>
#include <string>
#include<d3d12.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include"DirectXCommon.h"
#include<vector>
#include<memory>

class TextureManager {
private:
	//テクスチャ1枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource>resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle;
	};

	std::vector<TextureData>textureDatas;

	/*DirectX::ScratchImage image_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr<ID3D12Resource>textureResource_;
	DirectX::ScratchImage mipImage_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandle_;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_;*/

	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResources_;
	//std::vector<DirectX::ScratchImage> mipImages_;
	//std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> textureSrvHandles_;
	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

public:
	
	static uint32_t kSRVIndexTop;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
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

	  /// <summary>
	  /// 終了
	  /// </summary>
	  void Finalize();

	 D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;
private:
	static TextureManager* instance;
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

};
