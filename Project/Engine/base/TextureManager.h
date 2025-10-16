#pragma once
#include <d3d12.h>
#include <DirectXTex.h>
#include <string>
#include <wrl.h>
/// std
#include <unordered_map>

class DirectXCommon;
class SrvManager;
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

public:
    static TextureManager* GetInstance();

    /// 初期化
    void Init(DirectXCommon* dxCommon, SrvManager* srvManager);

    /// Textureデータの読み込み
    DirectX::ScratchImage LoadTextureFile(const std::string& filePath);
    uint32_t LoadTexture(const std::string& filePath);

    /// Resourceデータの作成、データの転送
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureDate(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    void Finalize();

private:
    static TextureManager* instance;
    TextureManager()                                 = default;
    ~TextureManager()                                = default;
    TextureManager(const TextureManager&)            = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    std::unordered_map<std::string, TextureData> textureDates_;

    DirectXCommon* directXCommon_;
    SrvManager* pSrvManager_;

public:
    ///==========================================================
    /// Getter method
    ///==========================================================

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const uint32_t& index) const;
    const DirectX::TexMetadata& GetMetaData(const uint32_t& textureIndex);
    uint32_t GetSrvIndex(const std::string& filePath);
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);
};
