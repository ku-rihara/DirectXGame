#include "DirectXCommon.h"
//function
#include"Convert.h"

//
#include"Vector4.h"
#include<format>
#include<cassert>


//
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

IDxcBlob* CompileShader(
	//CompilerするShaderファイルパス
	const std::wstring& filePath,
	//Compilerに使用するprofile
	const wchar_t* profile,
	//初期化で生成したものを3つ
	IDxcUtils* dxcUtils_,
	IDxcCompiler3* dxcCompiler_,
	IDxcIncludeHandler* includeHandler_) {

	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知

	//2,Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler_,//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//3,警告、エラーが出ていないかを確認する
	//警告、エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ
		assert(false);
	}

	//4,Compile結果を受け取って返す
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderSource->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Init(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	/// DXGIデバイス初期化
	DXGIDeviceInit();

	/// コマンド関連初期化
	CommandInit();

	//スワップチェーンの作成
	CreateSwapChain();

	//レンダーターゲットビューの生成
	CreateRenderTargetView();

	//フェンス生成
	CreateFence();

	//dxCompilerの初期化
	dxcCompilerInit();
}

void DirectXCommon::DXGIDeviceInit() {

	//DXGIファクトリーの生成
	dxgiFactory_ = nullptr;
	//HRESULTはWindows系のエラーコード
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない
	//場合が多いのでassertにしておく
	assert(SUCCEEDED(hr_));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	 useAdapter_ = nullptr;
	//いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {

		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力。wstringの方
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;
	}
	assert(useAdapter_ != nullptr);

	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr_)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成が上手く行かなかったので起動出来ない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		infoQueue->Release();
	}
#endif 
}



void DirectXCommon::CommandInit() {
	//コマンドキューを作成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成が上手くいかなかったので起動出来ない
	assert(SUCCEEDED(hr_));

	//コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr_ = GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成が上手くいかなかったので起動出来ない
	assert(SUCCEEDED(hr_));

	//コマンドリストを生成する
	commandList_ = nullptr;
	hr_ = GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_, nullptr, IID_PPV_ARGS(&commandList_));

	//コマンドリストの生成がうまくいかなかったので起動出来ない
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateSwapChain() {
	//スワップチェーンを生成する
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = backBufferWidth_;//画面の幅
	swapChainDesc.Height = backBufferHeight_;//画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニターに写したら、中身を破棄

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr_ = GetDxgiFactory()->CreateSwapChainForHwnd(GetCommandQueue(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain_));
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateRenderTargetView() {
	//ディスクリプタヒープの生成
	rtvDescriptorHeap_ = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = 2;//ダブルバッファ用に2つ。
	hr_ = GetDevice()->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));

	//ディスクリプタヒープが作れなかったので起動出来ない
	assert(SUCCEEDED(hr_));

	//SwapChainからResourceを引っ張ってくる
	for (int i = 0; i < 2; i++) {
		swapChainResources_[i] = nullptr;
		hr_ = GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));

		//うまく取得できなければ起動できない
		assert(SUCCEEDED(hr_));
	}

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む

	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	//まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	GetDevice()->CreateRenderTargetView(swapChainResources_[0], &rtvDesc, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	GetDevice()->CreateRenderTargetView(swapChainResources_[1], &rtvDesc, rtvHandles_[1]);
}

void DirectXCommon::CreateFence() {
	//初期値0でFenceを作る
	 fence_ = nullptr;
	 fenceValue_ = 0;
	hr_ = GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//FenceのSignalを待つためのイベントを作成する
	 fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::dxcCompilerInit() {
	//dxcCompilerを初期化
	 dxcUtils_ = nullptr;
	 dxcCompiler_ = nullptr;
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	 includeHandler_ = nullptr;
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));

}

ID3D12Resource* DirectXCommon::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソース用設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	//ばっぱリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;//リソースのサイズ。今回はVector4を3頂点文
	//バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	//バッファの場合これにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際に頂点リソースを作る
	ID3D12Resource* result=nullptr;
	hr_ = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr_));

	return result;
}

void DirectXCommon::CreateGraphicPipelene() {

	//RootSignatureを作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameterを作成
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PxelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメーターの配列
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ
	
	//シリアライズしてバイナリにする
	signatureBlob_ = nullptr;
	 errorBlob_ = nullptr;
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	rootSignature_ = nullptr;
	hr_ = GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));

	//InputLayoutの設定を行う
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の色を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	 vertexShaderBlob_ = CompileShader(L"Object3d.VS.hlsl",
		L"vs_6_0", GetDxcUtils(), GetDxcCompiler(), GetIncludeHandler());
	assert(vertexShaderBlob_ != nullptr);

	 pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl",
		L"ps_6_0", GetDxcUtils(), GetDxcCompiler(), GetIncludeHandler());
	assert(pixelShaderBlob_ != nullptr);

	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_;
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
	//実際に生成
	 graphicsPipelineState_ = nullptr;
	hr_ = GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));

	//VertexBufferViewを作成する	
	vertexResource_=CreateBufferResource(GetDevice(),sizeof(Vector4)*3);
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

	//頂点リソースにデータを書き込む
	Vector4* vertexDate= nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));
	//左下
	vertexDate[0] = { -0.5f,-0.5f,0.0f,1.0f };
	//上
	vertexDate[1] = { 0.0f,0.5f,0.0f,1.0f };
	//右下
	vertexDate[2] = { 0.5f,-0.5f,0.0f,1.0f };

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(GetDevice(), sizeof(Vector4));
	//マテリアルにデータを書き込む
	Vector4* materialDate = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//今回は赤を書き込む
	*materialDate = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	//ビューポート
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApp::kWindowWidth;
	viewport_.Height = WinApp::kWindowHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//シザー矩形
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right= WinApp::kWindowWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom= WinApp::kWindowHeight;
}

//フレーム開始
void DirectXCommon::ScreenClear() {
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = GetSwapChain()->GetCurrentBackBufferIndex();

	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = GetSwapChainResources(backBufferIndex);
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	GetCommandList()->ResourceBarrier(1, &barrier_);

	//描画先のRTVを設定する
	GetCommandList()->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色、RGBAの順
	GetCommandList()->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);
	//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること

	GetCommandList()->RSSetViewports(1, &viewport_);
	GetCommandList()->RSSetScissorRects(1, &scissorRect_);
	//RootSignatureを設定
	GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	GetCommandList()->SetPipelineState(graphicsPipelineState_);
	GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定
	GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//描画(DrawCall/ドローコール)
	GetCommandList()->DrawInstanced(3, 1, 0, 0);

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter= D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	GetCommandList()->ResourceBarrier(1, &barrier_);

	hr_ = GetCommandList()->Close();
	assert(SUCCEEDED(hr_));
}

//フレーム終わり
void DirectXCommon::CommandKick() {
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { GetCommandList() };
	GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	//GPUとOSに画面の交換を行うよう通知する
	GetSwapChain()->Present(1, 0);
	//Fenceの値を更新
	SetFenceValueIncrement();
	//GPUがここまでたどりついた時に、Fenceの値を指定した値に代入するようにSignalを送る
	GetCommandQueue()->Signal(GetFence(), GetFenceValue());

	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (GetFence()->GetCompletedValue() < GetFenceValue()) {

		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		GetFence()->SetEventOnCompletion(GetFenceValue(), GetFenceEvent());
		//イベントを待つ
		WaitForSingleObject(GetFenceEvent(), INFINITE);
	}
	//次のフレーム用のコマンドリストを準備
	hr_ = GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = GetCommandList()->Reset(GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::ResourceLeakCheck() {
	//リリースリークチェック
	IDXGIDebug* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}

void DirectXCommon::ReleaseObject() {
	CloseHandle(GetFenceEvent());
	GetFence()->Release();
	GetRtvDescriptorHeap()->Release();
	GetSwapChainResources(0)->Release();
	GetSwapChainResources(1)->Release();
	GetSwapChain()->Release();
	GetCommandList()->Release();
	GetCommandAllocator()->Release();
	GetCommandQueue()->Release();
	GetDevice()->Release();
	GetUseAdapter()->Release();
	GetDxgiFactory()->Release();
	vertexResource_->Release();
	materialResource_->Release();
	graphicsPipelineState_->Release();
	signatureBlob_->Release();
	if (errorBlob_) {
		errorBlob_->Release();
	}
	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
#ifdef _DEBUG
	
	winApp_->GetDebugController()->Release();
#endif 
	CloseWindow(winApp_->GetHwnd());
}