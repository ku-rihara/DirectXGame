#pragma once
#include<wrl.h>
#include <string>
#include<d3d12.h>
#include<DirectXTex.h>
#include"Dx/DirectXCommon.h"
/// std
#include<unordered_map>

class SrvManager;
class TextureManager {
private:
	//テクスチャ1枚分のデータ
	struct TextureData {
		/*std::string filePath;*/
		DirectX::TexMetadata metadata;
		uint32_t index;
		uint32_t srvIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource>resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle;
	};

	std::unordered_map <std::string,TextureData> textureDatas_;

  /*  D3D12_GPU_DESCRIPTOR_HANDLE skyBoxHandle_;*/
	/// ohter class
	SrvManager* pSrvManager_;

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
	void Init(DirectXCommon* dxCommon,SrvManager*srvManager);
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



	///==========================================================
	///Getter method
	///==========================================================
	
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index)const;
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);
	uint32_t GetSrvIndex(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
   /* D3D12_GPU_DESCRIPTOR_HANDLE GetSkyBoxTexture() const {  return skyBoxHandle_;}*/

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

