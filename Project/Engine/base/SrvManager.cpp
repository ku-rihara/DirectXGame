#include"SrvManager.h"

const uint32_t SrvManager::kMaxSRVCount = 512;

  ///=========================================
  ///初期化
  ///=========================================
void SrvManager::Init(DirectXCommon* dxCommon) {
	this->dxCommon_ = dxCommon;

	// でスクリプタヒープ生成
	descriptorHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
	/// デスクリプタ一個分のサイズを取得して記録
	descriptorSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

  ///=========================================
  ///アロケータ
  ///=========================================
uint32_t  SrvManager::Allocate() {

	// return する番号を一旦記録
	int index = useIndex_;

	//次回の為に番号を1進める
	useIndex_++;

	//上で記録した番号をreturn 
	return useIndex_;
}

void SrvManager::PreDraw() {
	//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = {descriptorHeap_.Get()};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
}

///=========================================
/// GPU、CPUデスクリプタハンドル取得
 ///=========================================
D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

///=========================================
/// テクスチャ用SRV生成
///=========================================
void SrvManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = MipLevels;
	//生成
	dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}