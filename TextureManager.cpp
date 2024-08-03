#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Model.h"
#include"DirectXCommon.h"
#include"Convert.h"
#include<vector>

namespace {
	DirectXCommon* directXCommon_;
	ImGuiManager* imguiManager_;
	Model* model_;
}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image_);
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image_.GetImages(), image_.GetImageCount(), image_.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr < ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata) {

	//1,metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1個
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//2,利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う

	//3,Reourceを生成してreturnする
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));//作成するResourceポインタへのポインタ
    	assert(SUCCEEDED(hr));
		return resource.Get();
}
[[nodiscard]]
Microsoft::WRL::ComPtr < ID3D12Resource> TextureManager::UploadTextureDate(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages, Microsoft::WRL::ComPtr<ID3D12Device>device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList) {
	 
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource = directXCommon_->GetInstance()->CreateBufferResource(device.Get(), intermediateSize);
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

void TextureManager::Load(const std::string& textureFilePath) {
    directXCommon_ = DirectXCommon::GetInstance();
    imguiManager_ = ImGuiManager::GetInstance();

    mipImages_ = LoadTexture(textureFilePath);
    const DirectX::TexMetadata& metadata = mipImages_.GetMetadata();
    textureResource_ = CreateTextureResource(directXCommon_->GetDevice(), metadata);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    textureSrvHandleCPU_ = imguiManager_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    textureSrvHandleGPU_ = imguiManager_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

    textureSrvHandleCPU_.ptr += directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    textureSrvHandleGPU_.ptr += directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    directXCommon_->GetDevice()->CreateShaderResourceView(textureResource_.Get(), &srvDesc, textureSrvHandleCPU_);
	Microsoft::WRL::ComPtr < ID3D12Resource>intermediateResource=UploadTextureDate(textureResource_, mipImages_, directXCommon_->GetDevice(), directXCommon_->GetCommandList());
	directXCommon_->commandExecution(intermediateResource);
}