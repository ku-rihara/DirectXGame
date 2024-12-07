#pragma once

/// base
#include"base/DirectXCommon.h"

///SRV管理
class SrvManager {
private:

	///=========================================
	///private variant
	///=========================================

	/// other class
	DirectXCommon* dxCommon_ = nullptr;

	uint32_t descriptorSize_;                                    //SRV用のデスクリプタサイズ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;//SRV用デスクリプタ

	uint32_t useIndex_ = 0;//次に使用するインデックス

	/*/// structreBuffer
	D3D12_CPU_DESCRIPTOR_HANDLE  instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE  instancingSrvHandleGPU_;*/
	//リソースとデータ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResources_;

public:
	//最大SRV数
	static const uint32_t kMaxCount;

private:
	///=========================================
  /// private constructor / destructor
  ///=========================================
	SrvManager() = default;
	~SrvManager() = default;

	// コピーコンストラクタ・代入演算子を削除してシングルトンにする
	SrvManager(const SrvManager&) = delete;
	SrvManager& operator=(const SrvManager&) = delete;

public:

	///===================================================================
	/// public static method for Singleton
	///===================================================================
	static SrvManager* GetInstance();

	///===================================================================
	///public method
	///===================================================================
	void Init(DirectXCommon* dxCommon);

	// アロケータ
	uint32_t Allocate();

	// 描画前処理
	void PreDraw();

	bool IsAbleSecure();

	/// GPU、CPUデスクリプタハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// SRV生成(テクスチャ用、Structured Buffer用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structuredByteStride);

	///===================================================================
	///getter method
	///===================================================================

	uint32_t GetDescriptorSizeSRV()const { return descriptorSize_; }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap()const { return descriptorHeap_.Get(); }

	///===================================================================
	///setter method
	///===================================================================
	void SetSrvDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srv) { this->descriptorHeap_ = srv; }

};