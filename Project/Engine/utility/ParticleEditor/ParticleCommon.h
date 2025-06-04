#pragma once
#include"Dx/DirectXCommon.h"
#include"base/SrvManager.h"
#include "base/Material.h"


class ParticleCommon {
public:
  
private:

	///=========================================
    /// private variant
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

	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateAdd_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateNone_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateMultiply_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateSubtractive_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateScreen_;

	// depth
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;
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
	void PreDraw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode);

	///==============================================
	///getter method
	///==============================================

	DirectXCommon* GetDxCommon()const { return pDxCommon_; }
	ID3D12PipelineState* GetGrahipcsPipeLileStateAdd()const { return graphicsPipelineStateAdd_.Get(); }
	ID3D12PipelineState* GetGrahipcsPipeLileStateNone()const { return graphicsPipelineStateNone_.Get(); }
	ID3D12RootSignature* GetRootSignature()const { return rootSignature_.Get(); }
};