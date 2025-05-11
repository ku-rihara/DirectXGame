#pragma once

#include"DirectXCommon.h"


class SkyBoxRenderer {

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


Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
	

	// depth
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_;

private:
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();

public://メンバ関数
	//共通描画処理
	void PreDraw(ID3D12GraphicsCommandList* commandList);
    void SetPiplelineState(ID3D12GraphicsCommandList* commandList);
	//初期化
	void Init(DirectXCommon* dxCommon);

	static SkyBoxRenderer* GetInstance();

	//rootSignature
	ID3D12RootSignature* GetRootSignature()const { return rootSignature_.Get(); }


};

