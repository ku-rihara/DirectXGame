#include"Object3DCommon.h"
//Function
#include"function/Log.h"
#include<cassert>
#include<string>


Object3DCommon* Object3DCommon::GetInstance() {
	static Object3DCommon instance;
	return &instance;
}


void Object3DCommon::Init(DirectXCommon* dxCommon) {

	//引数で受けとる
	dxCommon_ = dxCommon;
	//グラフィックスパイプラインの生成
	CreateGraphicsPipeline();

}

void Object3DCommon::CreateGraphicsPipeline() {

	HRESULT hr = 0;


	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE; // アルファテストを無効に
	blendDesc.IndependentBlendEnable = FALSE; // 各レンダーターゲットのブレンドを独立させない

	// レンダーターゲットの設定
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	// ソースブレンド、デスティネーションブレンド、ブレンドオペレーションの設定
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースのアルファ
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 1 - ソースのアルファ
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // 加算

	// アルファのソースとデスティネーションのブレンド設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // アルファをそのまま使用
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // デスティネーションのアルファは無視
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // 加算

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の色を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/Object3d.VS.hlsl",
		L"vs_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/Shader/Object3d.PS.hlsl",
		L"ps_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
	assert(pixelShaderBlob_ != nullptr);

	//ShaderをコンパイルするParticle
	vertexShaderBlobParticle_ = dxCommon_->CompileShader(L"resources/Shader/Particle.VS.hlsl",
		L"vs_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
	assert(vertexShaderBlobParticle_ != nullptr);

	pixelShaderBlobParticle_ = dxCommon_->CompileShader(L"resources/Shader/Particle.PS.hlsl",
		L"ps_6_0", dxCommon_->GetDxcUtils(), dxCommon_->GetDxcCompiler(), dxCommon_->GetIncludeHandler());
	assert(pixelShaderBlobParticle_ != nullptr);

	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = dxCommon_->GetDepthStencilDesc();
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Particle用のPSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescParticle{};
	graphicsPipelineStateDescParticle.pRootSignature = rootSignatureParticle_.Get();
	graphicsPipelineStateDescParticle.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDescParticle.VS = { vertexShaderBlobParticle_->GetBufferPointer(), vertexShaderBlobParticle_->GetBufferSize() };
	graphicsPipelineStateDescParticle.PS = { pixelShaderBlobParticle_->GetBufferPointer(), pixelShaderBlobParticle_->GetBufferSize() };
	graphicsPipelineStateDescParticle.BlendState = blendDesc;
	graphicsPipelineStateDescParticle.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDescParticle.NumRenderTargets = 1;
	graphicsPipelineStateDescParticle.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDescParticle.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDescParticle.SampleDesc.Count = 1;
	graphicsPipelineStateDescParticle.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDescParticle.DepthStencilState = dxCommon_->GetDepthStencilDesc();
	graphicsPipelineStateDescParticle.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	graphicsPipelineState_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));

	//実際に生成(パーティクル)
	graphicsPipelineStateParticle_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDescParticle, IID_PPV_ARGS(&graphicsPipelineStateParticle_));
	assert(SUCCEEDED(hr));
	/*model_->CreateModel();*/
	/*sprite_->CreateSprite();*/

}

void Object3DCommon::CreateRootSignature() {
	HRESULT hr = 0;
	//RootSignatureを作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pStaticSamplers = staticSamplers_;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers_);

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameterを作成
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PxelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	// Lambart
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;
	//Half Lambart
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 2;
	//PointLight
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].Descriptor.ShaderRegister = 3;
	//SpotLight
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].Descriptor.ShaderRegister = 4;

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメーターの配列
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Particle**************************************************************************************************
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignatureParticle{};

	descriptionRootSignatureParticle.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignatureParticle.pStaticSamplers = staticSamplers_;
	descriptionRootSignatureParticle.NumStaticSamplers = _countof(staticSamplers_);

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER RootParameterParticle[3] = {};
	RootParameterParticle[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParameterParticle[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParameterParticle[0].Descriptor.ShaderRegister = 0;

	RootParameterParticle[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameterParticle[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	RootParameterParticle[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	RootParameterParticle[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	RootParameterParticle[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameterParticle[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	RootParameterParticle[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	RootParameterParticle[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	descriptionRootSignatureParticle.pParameters = RootParameterParticle;
	descriptionRootSignatureParticle.NumParameters = _countof(RootParameterParticle);


	//Object*************************************************************************************************************************
	//シリアライズしてバイナリにする
	signatureBlob_ = nullptr;
	errorBlob_ = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	rootSignature_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
	//Particle*************************************************************************************************************************************
	signatureBlobParticle_ = nullptr;
	errorBlobParticle_ = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignatureParticle, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlobParticle_, &errorBlobParticle_);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlobParticle_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	rootSignatureParticle_ = nullptr;
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlobParticle_->GetBufferPointer(), signatureBlobParticle_->GetBufferSize(), IID_PPV_ARGS(&rootSignatureParticle_));
	assert(SUCCEEDED(hr));

}


void Object3DCommon::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// RootSignatureを設定
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	// PipelineStateを設定
	commandList->SetPipelineState(graphicsPipelineState_.Get());

}

void Object3DCommon :: PreDrawParticle(ID3D12GraphicsCommandList* commandList) {

	commandList->SetGraphicsRootSignature(rootSignatureParticle_.Get());
	commandList->SetPipelineState(graphicsPipelineStateParticle_.Get());
}