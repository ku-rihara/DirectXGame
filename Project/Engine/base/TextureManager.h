#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <string>
#include <wrl.h>
/// std
#include <unordered_map>

namespace KetaEngine {

class DirectXCommon;
class SrvManager;
/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TextureManager {
private:
    // テクスチャ1枚分のデータ
    struct TextureData {
        DirectX::TexMetadata metadata;
        uint32_t index;
        uint32_t srvIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle;
    };

private:
    static TextureManager* instance;
    TextureManager()                                 = default;
    ~TextureManager()                                = default;
    TextureManager(const TextureManager&)            = delete;
    TextureManager& operator=(const TextureManager&) = delete;

public:
    static TextureManager* GetInstance();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    /// <param name="srvManager">SrvManager</param>
    void Init(DirectXCommon* dxCommon, SrvManager* srvManager);

    /// <summary>
    /// Textureデータの読み込み
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <returns>ScratchImage</returns>
    DirectX::ScratchImage LoadTextureFile(const std::string& filePath);

    /// <summary>
    /// テクスチャの読み込み
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <returns>テクスチャインデックス</returns>
    uint32_t LoadTexture(const std::string& filePath);

    /// <summary>
    /// Resourceデータの作成
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="metadata">メタデータ</param>
    /// <returns>リソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

    /// <summary>
    /// テクスチャデータのアップロード
    /// </summary>
    /// <param name="texture">テクスチャリソース</param>
    /// <param name="mipImages">ミップマップイメージ</param>
    /// <param name="device">デバイス</param>
    /// <param name="commandList">コマンドリスト</param>
    /// <returns>中間リソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureDate(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    void Finalize(); //< 終了処理

private:
    std::unordered_map<std::string, TextureData> textureDates_;

    DirectXCommon* directXCommon_;
    SrvManager* pSrvManager_;

public:
    ///==========================================================
    /// Getter method
    ///==========================================================

    /// <summary>
    /// テクスチャハンドルの取得
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <returns>GPUディスクリプタハンドル</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(uint32_t index) const;

    /// <summary>
    /// メタデータの取得
    /// </summary>
    /// <param name="textureIndex">テクスチャインデックス</param>
    /// <returns>メタデータ</returns>
    const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

    /// <summary>
    /// SRVインデックスの取得
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <returns>SRVインデックス</returns>
    uint32_t GetSrvIndex(const std::string& filePath);

    /// <summary>
    /// SRVハンドルGPUの取得
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <returns>GPUディスクリプタハンドル</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
};

}; // KetaEngine
