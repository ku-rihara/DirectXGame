#include "base/TextureManager.h"
#include "2d/ImGuiManager.h"
/// srv
#include"SrvManager.h"
//function
#include "function/Convert.h"
#include <vector>
#include <d3dx12.h>



// descriptorHeapIndex_ の初期化
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::instance = nullptr;

namespace {
	DirectXCommon* directXCommon_;
	ImGuiManager* imguiManager_;
}
//インスタンス取得
TextureManager* TextureManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Init(DirectXCommon* dxCommon, SrvManager* srvManager) {
	directXCommon_ = dxCommon;
	pSrvManager_ = srvManager;
	//SRVと同様
	textureDatas_.reserve(SrvManager::kMaxCount);
}

DirectX::ScratchImage TextureManager::LoadTextureFile(const std::string& filePath) {
	// テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミニマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {
	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 2. 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 3. Resourceを生成してreturnする
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource.Get();
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureDate(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = directXCommon_->GetInstance()->CreateBufferResource(device.Get(), intermediateSize);
	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource.Get();
}


uint32_t TextureManager::LoadTexture(const std::string& filePath) {
	
	// 読み込み済みテクスチャを検索
	if (textureDatas_.contains(filePath)) {
		return textureDatas_[filePath].srvIndex; // インデックスを返す
	}

	// テクスチャ上限枚数をチェック
	assert(pSrvManager_->IsAbleSecure());

	// テクスチャファイルを読み、MipMapの作成
	DirectX::ScratchImage mipImage = LoadTextureFile(filePath);

	TextureData& textureData = textureDatas_[filePath];

	// メタデータとリソースの初期化
	textureData.metadata = mipImage.GetMetadata();
	textureData.resource = CreateTextureResource(directXCommon_->GetDevice(), textureData.metadata);

	// SRV確保
	textureData.srvIndex = pSrvManager_->Allocate();
	textureData.srvCPUHandle = pSrvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvGPUHandle = pSrvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	// SRVの作成
	pSrvManager_->CreateSRVforTexture2D(
		textureData.srvIndex,
		textureData.resource.Get(),
		textureData.metadata.format,
		static_cast<UINT>(textureData.metadata.mipLevels)
	);

	// データをアップロード
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureDate(
		textureData.resource.Get(),
		mipImage,
		directXCommon_->GetDevice(),
		directXCommon_->GetCommandList()
	);

	// コマンド実行
	directXCommon_->commandExecution(intermediateResource);

	// 新しく読み込んだテクスチャのインデックスを返す
	return textureData.srvIndex;
}


void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}


const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex) {
	// 範囲外チェック
	assert(textureIndex < textureDatas_.size());

	// 指定されたインデックスのエントリを取得
	auto it = textureDatas_.begin();
	std::advance(it, textureIndex);

	// 値部分の参照を返す
	return it->second.metadata;
}



D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetTextureHandle(uint32_t index) const {

	assert(index < textureDatas_.size());

	// インデックスに対応するエントリを探す
	auto it = textureDatas_.begin();
	std::advance(it, index);

	const TextureData& textureData = it->second;  // マップの値部分を取得
	return textureData.srvGPUHandle;
}


uint32_t TextureManager::GetSrvIndex(const std::string& filePath) {
	// ファイルパスに基づいてインデックスを検索
	uint32_t index = 0;
	for (const auto& [key, textureData] : textureDatas_) {
		if (key == filePath) {
			return index;
		}
		++index;
	}
	// 見つからない場合はエラー
	assert(false);
	return UINT32_MAX;
}


D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath) {
	// サービスインデックスを取得
	uint32_t index = GetSrvIndex(filePath);

	// ハンドルを返す
	return GetTextureHandle(index);
}



////0番はImGuiが使っているため1番から使う
	//uint32_t srvIndex = static_cast<uint32_t>(textureDatas_.size()-1)+kSRVIndexTop;
	//textureData.srvCPUHandle = directXCommon_->GetCPUDescriptorHandle(imguiManager_->GetSrvDescriptorHeap(), directXCommon_->GetDescriptorSizeSRV(), uint32_t(textureDatas_.size()) +srvIndex);
	//textureData.srvGPUHandle = directXCommon_->GetGPUDescriptorHandle(imguiManager_->GetSrvDescriptorHeap(), directXCommon_->GetDescriptorSizeSRV(), uint32_t(textureDatas_.size()) +srvIndex);

	//// シェーダリソースビューの設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = textureData.metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	////生成
	//directXCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvCPUHandle);
