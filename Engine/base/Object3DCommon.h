#pragma once

#include"DirectXCommon.h"


class Object3DCommon {
private://メンバ変数

	DirectXCommon* dxCommon_;

private://メンバ関数
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];
	//グラフィックパイプライン関連
//object
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob>vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob>pixelShaderBlob_;
	//particle
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatureParticle_;
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlobParticle_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlobParticle_;
	Microsoft::WRL::ComPtr<IDxcBlob>vertexShaderBlobParticle_;
	Microsoft::WRL::ComPtr<IDxcBlob>pixelShaderBlobParticle_;
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateParticle_;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();

public://メンバ関数
	//共通描画処理
	void PreDraw(ID3D12GraphicsCommandList* commandList);
	void PreDrawParticle(ID3D12GraphicsCommandList* commandList);

	//初期化
	void Init(DirectXCommon* dxCommon);

static	Object3DCommon* GetInstance();

	//getter
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	//rootSignature
	ID3D12PipelineState* GetGrahipcsPipeLileState()const { return graphicsPipelineState_.Get(); }
	ID3D12RootSignature* GetRootSignature()const { return rootSignature_.Get(); }

	ID3D12PipelineState* GetGrahipcsPipeLileStateParticle()const { return graphicsPipelineStateParticle_.Get(); }
	ID3D12RootSignature* GetRootSignatureParticle()const { return rootSignatureParticle_.Get(); }
};

