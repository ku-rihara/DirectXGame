#pragma once
#include"base/DirectXCommon.h"
#include"base/SrvManager.h"

class ParticleCommon {
private:

	///=========================================
	///private variant
	///=========================================

	// ohter class
	SrvManager* pSrvManager_;
	DirectXCommon* pDxCommon_;

	//particle
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob>vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob>pixelShaderBlob_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState_;

private:

	///==============================================
	///private method
	///==============================================


	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();

public:
	
	static	ParticleCommon* GetInstance();

	///==============================================
	///public method
	///==============================================

	//初期化
	void Init(DirectXCommon* dxCommon);
	void PreDraw(ID3D12GraphicsCommandList* commandList);

	///==============================================
	///getter method
	///==============================================

	DirectXCommon* GetDxCommon()const { return pDxCommon_; }
	ID3D12PipelineState* GetGrahipcsPipeLileState()const { return graphicsPipelineState_.Get(); }
	ID3D12RootSignature* GetRootSignature()const { return rootSignature_.Get(); }
};