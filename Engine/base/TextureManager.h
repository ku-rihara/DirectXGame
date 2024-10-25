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

	std::vector<TextureData>textureDatas_;


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
  /// テクスチャハンドル取得関数
  /// </summary>
  /// <param name="index"></param>
  /// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;

	/// <summary>
	/// メタデータを取得
	/// </summary>
	/// <param name="textureIndex"></param>
	/// <returns></returns>
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
private:
	static TextureManager* instance;
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

};

