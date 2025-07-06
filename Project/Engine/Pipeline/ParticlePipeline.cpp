#include"ParticlePipeline.h"
//Function
#include"function/Log.h"
#include"base/Material.h"
#include<cassert>
#include<string>


ParticlePipeline* ParticlePipeline::GetInstance() {
	static ParticlePipeline instance;
	return &instance;
}


void ParticlePipeline::Init(DirectXCommon* dxCommon) {

	//引数で受けとる
	pDxCommon_ = dxCommon;
	

	//グラフィックスパイプラインの生成
	CreateGraphicsPipeline();

}

void ParticlePipeline::CreateGraphicsPipeline() {

	HRESULT hr = 0;

	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipMapを使う
	staticSamplers_[0].ShaderRegister = 0;//レジスタ番号０
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pxelShaderで使う

	CreateRootSignature();

	//InputLayoutの設定を行う
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	// ADD ブレンド設定
	D3D12_BLEND_DESC blendDescAdd = {};
	blendDescAdd.RenderTarget[0].BlendEnable = TRUE;
	blendDescAdd.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAdd.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescAdd.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAdd.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAdd.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescAdd.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAdd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 通常（ブレンドなし）
	D3D12_BLEND_DESC blendDescAlpha = {};
	blendDescAlpha.RenderTarget[0].BlendEnable = TRUE; // ブレンド有効化
	blendDescAlpha.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDescAlpha.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescAlpha.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescAlpha.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescAlpha.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	// 乗算ブレンド設定
	D3D12_BLEND_DESC blendDescMultiply = {};
	blendDescMultiply.RenderTarget[0].BlendEnable = TRUE;
	blendDescMultiply.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDescMultiply.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	blendDescMultiply.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescMultiply.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	blendDescMultiply.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	blendDescMultiply.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescMultiply.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 減算ブレンド設定
	D3D12_BLEND_DESC blendDescSubtractive = {};
	blendDescSubtractive.RenderTarget[0].BlendEnable = TRUE;
	blendDescSubtractive.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDescSubtractive.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDescSubtractive.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDescSubtractive.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescSubtractive.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescSubtractive.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescSubtractive.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// スクリーンブレンド設定
	D3D12_BLEND_DESC blendDescScreen = {};
	blendDescScreen.RenderTarget[0].BlendEnable = TRUE;
	blendDescScreen.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDescScreen.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
	blendDescScreen.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDescScreen.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDescScreen.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDescScreen.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDescScreen.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の色を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//DepthStencilStateの設定-------------------------------------
   //Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	//書き込みする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//ShaderをコンパイルするParticle
	vertexShaderBlob_ = pDxCommon_->CompileShader(L"resources/Shader/Particle.VS.hlsl",
		L"vs_6_0", pDxCommon_->GetDxcUtils(), pDxCommon_->GetDxcCompiler(), pDxCommon_->GetIncludeHandler());
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = pDxCommon_->CompileShader(L"resources/Shader/Particle.PS.hlsl",
		L"ps_6_0", pDxCommon_->GetDxcUtils(), pDxCommon_->GetDxcCompiler(), pDxCommon_->GetIncludeHandler());
	assert(pixelShaderBlob_ != nullptr);

	// PSO作成用関数
	auto CreatePSO = [&](D3D12_BLEND_DESC& blendDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pso) {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
		graphicsPipelineStateDesc.BlendState = blendDesc;
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		hr = pDxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pso));
		assert(SUCCEEDED(hr));
		};

	// 各ブレンドモードのPSOを作成
	CreatePSO(blendDescAdd, graphicsPipelineStateAdd_);
	CreatePSO(blendDescAlpha, graphicsPipelineStateNone_);
	CreatePSO(blendDescMultiply, graphicsPipelineStateMultiply_);
	CreatePSO(blendDescSubtractive, graphicsPipelineStateSubtractive_);
	CreatePSO(blendDescScreen, graphicsPipelineStateScreen_);

}

void ParticlePipeline::CreateRootSignature() {
	HRESULT hr = 0;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pStaticSamplers = staticSamplers_;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameter[3] = {};
	RootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParameter[0].Descriptor.ShaderRegister = 0;

	RootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	RootParameter[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	RootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	RootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	RootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	descriptionRootSignature.pParameters = RootParameter;
	descriptionRootSignature.NumParameters = _countof(RootParameter);


	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	rootSignature_ = nullptr;
	hr = pDxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

}

void ParticlePipeline::PreDraw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) {
	commandList->SetGraphicsRootSignature(rootSignature_.Get());

	switch (blendMode) {
	case BlendMode::None:
		commandList->SetPipelineState(graphicsPipelineStateNone_.Get());
		break;
    case BlendMode::Add:
        commandList->SetPipelineState(graphicsPipelineStateAdd_.Get());
        break;
	case BlendMode::Multiply:
		commandList->SetPipelineState(graphicsPipelineStateMultiply_.Get());
		break;
	case BlendMode::Subtractive:
		commandList->SetPipelineState(graphicsPipelineStateSubtractive_.Get());
		break;
	case BlendMode::Screen:
		commandList->SetPipelineState(graphicsPipelineStateScreen_.Get());
		break;
	}
}
