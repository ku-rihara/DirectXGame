#include "DirectXCommon.h"
#include "2d/ImGuiManager.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "base/TextureManager.h"

// function
#include "Frame/Frame.h"
#include "function/Convert.h"
#include "function/Log.h"
//

#include <cassert>

#include <d3d12.h>
#include <imgui_impl_dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

DirectXCommon* DirectXCommon::GetInstance() {
    static DirectXCommon instance;
    return &instance;
}

///==========================================================
///  シェーダーコンパイル
///==========================================================
Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(
    // CompilerするShaderファイルパス
    const std::wstring& filePath,
    // Compilerに使用するprofile
    const wchar_t* profile,
    // 初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler) {

    // これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader,path:{}\n", filePath, profile)));
    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr                     = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら止める
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

    // 2,Compileする
    LPCWSTR arguments[] = {
        filePath.c_str(), // コンパイル対象のhlslファイル名
        L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile, // ShaderProfileの設定
        L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
        L"-Od", // 最適化を外しておく
        L"-Zpr", // メモリレイアウトは行優先
    };
    // 実際にShaderをコンパイルする
    Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
    hr                                              = dxcCompiler->Compile(
        &shaderSourceBuffer, // 読み込んだファイル
        arguments, // コンパイルオプション
        _countof(arguments), // コンパイルオプションの数
        includeHandler, // includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );
    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));

    // 3,警告、エラーが出ていないかを確認する
    // 警告、エラーが出てたらログに出して止める
    Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        // 警告・エラーダメ
        assert(false);
    }

    // 4,Compile結果を受け取って返す
    Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
    hr                                          = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.GetAddressOf()), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded,path:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();

    // 実行用のバイナリを返却
    return shaderBlob.Get();
}

///==========================================================
///	初期化
///==========================================================
void DirectXCommon::Init(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

    winApp_           = winApp;
    backBufferWidth_  = backBufferWidth;
    backBufferHeight_ = backBufferHeight;
    srvManager_       = SrvManager::GetInstance();
    rtvManager_       = RtvManager::GetInstance();

    // FIP固定初期化
    // InitFixFPS();
    Frame::Init();

    /// DXGIデバイス初期化
    DXGIDeviceInit();
    /*srvManager_->Init(this);*/

    /// コマンド関連初期化
    CommandInit();
}

void DirectXCommon::InitRenderingResources() {

    descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    // スワップチェーンの作成
    CreateSwapChain();

    // レンダーターゲットビューの生成
    CreateRenderTargetView();

    // 深度バッファの作成
    CreateDepthBuffer();

    // フェンス生成
    CreateFence();

    // dxCompilerの初期化
    dxcCompilerInit();

    CreateGraphicPipelene();

    imguiManager_   = ImGuiManager::GetInstance();
    textureManager_ = TextureManager::GetInstance();
}

///==========================================================
/// DXGIデバイス初期化
///==========================================================
void DirectXCommon::DXGIDeviceInit() {

    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコード
    hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない
    // 場合が多いのでassertにしておく
    assert(SUCCEEDED(hr_));

    // 使用するアダプタ用の変数。最初にnullptrを入れておく
    useAdapter_ = nullptr;
    // いい順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_))
                     != DXGI_ERROR_NOT_FOUND;
        ++i) {

        // アダプターの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr_ = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr_)); // 取得できないのは一大事
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタの情報をログに出力。wstringの方
            Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter_ = nullptr;
    }
    assert(useAdapter_ != nullptr);

    device_ = nullptr;
    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0};

    const char* featureLevelStrings[] = {"12.2", "12.1", "12.0"};
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(hr_)) {
            // 生成できたのでログ出力を行ってループを抜ける
            Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスの生成が上手く行かなかったので起動出来ない
    assert(device_ != nullptr);
    Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバいエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE};
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs        = _countof(denyIds);
        filter.DenyList.pIDList       = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
        // 解放
        infoQueue->Release();
    }
#endif
}

///==========================================================
/// コマンド初期化
///==========================================================
void DirectXCommon::CommandInit() {
    // コマンドキューを作成する
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr_ = GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成が上手くいかなかったので起動出来ない
    assert(SUCCEEDED(hr_));

    // コマンドアロケータを生成する
    commandAllocator_ = nullptr;
    hr_               = GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    // コマンドアロケータの生成が上手くいかなかったので起動出来ない
    assert(SUCCEEDED(hr_));

    // コマンドリストを生成する
    commandList_ = nullptr;
    hr_          = GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));

    // コマンドリストの生成がうまくいかなかったので起動出来ない
    assert(SUCCEEDED(hr_));
}

///==========================================================
/// スワップチェーン作成
///==========================================================
void DirectXCommon::CreateSwapChain() {
    // スワップチェーンを生成する
    swapChain_                      = nullptr;
    swapChainDesc_.Width            = backBufferWidth_; // 画面の幅
    swapChainDesc_.Height           = backBufferHeight_; // 画面の高さ
    swapChainDesc_.Format           = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
    swapChainDesc_.SampleDesc.Count = 1; // マルチサンプルしない
    swapChainDesc_.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc_.BufferCount      = 2; // ダブルバッファ
    swapChainDesc_.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニターに写したら、中身を破棄

    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr_));
}

///==========================================================
/// レンダーターゲットビューの作成
///==========================================================
void DirectXCommon::CreateRenderTargetView() {

    // SwapChainからResourceを引っ張ってくる
    for (int i = 0; i < 2; i++) {
        swapChainResources_[i] = nullptr;
        hr_                    = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
        // うまく取得できなければ起動できない
        assert(SUCCEEDED(hr_));
    }

    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // RTV作成：SwapChainの2枚分
    for (int i = 0; i < 2; ++i) {
        uint32_t index = rtvManager_->Allocate();
      /*  rtvHandles_[i] = rtvManager_->GetCPUDescriptorHandle(index);*/
        rtvManager_->CreateRTV(index, swapChainResources_[i].Get(), &rtvDesc);
    }

    // 3枚目：レンダーターゲット（テクスチャ用）
    Vector4 color          = {0.1f, 0.1f, 0.1f, 1.0f};
    renderTextureResource_ = CreateRenderTextureResource(
        device_, WinApp::kWindowWidth, WinApp::kWindowHeight,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, color);

    uint32_t index = rtvManager_->Allocate();
  /*  rtvHandles_[2] = rtvManager_->GetCPUDescriptorHandle(index);*/
    rtvManager_->CreateRTV(index, renderTextureResource_.Get(), &rtvDesc);

    //// ディスクリプタヒープの生成
    // rtvDescriptorHeap_ = CreateDescriptorHeap(GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);
    //// SwapChainからResourceを引っ張ってくる
    // for (int i = 0; i < 2; i++) {
    //     swapChainResources_[i] = nullptr;
    //     hr_                    = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
    //     // うまく取得できなければ起動できない
    //     assert(SUCCEEDED(hr_));
    // }

    //// RTVの設定
    // rtvDesc_.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
    // rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む

    //// まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
    // rtvHandles_[0] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 0);
    // rtvHandles_[1] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 1);
    // rtvHandles_[2] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 2);

    // GetDevice()->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
    // GetDevice()->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);

    //// 3
    // const Vector4 color = {0.1f, 0.1f, 0.1f, 1.0f};

    // renderTextureResource_ = CreateRenderTextureResource(
    //     device_, WinApp::kWindowWidth, WinApp::kWindowHeight,
    //     DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, color);

    // device_->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc_, rtvHandles_[2]);
}

///==========================================================
/// 深度バッファ生成
///==========================================================
void DirectXCommon::CreateDepthBuffer() {
    depthStencilResource_ = CreateDepthStencilTextureResource(device_, WinApp::kWindowWidth, WinApp::kWindowHeight);

    // DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dsvDescriptorHeap_ = InitializeDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture
    // DSVHeapの先頭にDSVを作る
    device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

///==========================================================
/// フェンス生成
///==========================================================
void DirectXCommon::CreateFence() {
    // 初期値0でFenceを作る
    fence_      = nullptr;
    fenceValue_ = 0;
    hr_         = GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr_));

    // FenceのSignalを待つためのイベントを作成する
    fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent_ != nullptr);
}

///==========================================================
/// dxcCompiler初期化
///==========================================================
void DirectXCommon::dxcCompilerInit() {
    // dxcCompilerを初期化
    dxcUtils_    = nullptr;
    dxcCompiler_ = nullptr;
    hr_          = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(hr_));
    hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(hr_));

    // 現時点でincludeはしないが、includeに対応するための設定を行っておく
    includeHandler_ = nullptr;
    hr_             = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr_));
}

///==========================================================
/// グラフィックパイプラインの生成
///==========================================================
void DirectXCommon::CreateGraphicPipelene() {

    // ビューポート
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport_.Width    = WinApp::kWindowWidth;
    viewport_.Height   = WinApp::kWindowHeight;
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;

    // シザー矩形
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect_.left   = 0;
    scissorRect_.right  = WinApp::kWindowWidth;
    scissorRect_.top    = 0;
    scissorRect_.bottom = WinApp::kWindowHeight;
}

void DirectXCommon::PreRenderTexture() {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    // 現在の状態がすでにRENDER_TARGETでない場合のみ遷移
    if (renderTextureCurrentState_ != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        PutTransitionBarrier(renderTextureResource_.Get(),
            renderTextureCurrentState_, D3D12_RESOURCE_STATE_RENDER_TARGET);
        renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(2);

    commandList_->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    // レンダーターゲットと深度ステンシルビューをクリア
    commandList_->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);
    commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    // ビューポートとシザー矩形を設定
    commandList_->RSSetViewports(1, &viewport_);
    commandList_->RSSetScissorRects(1, &scissorRect_);
}

///==========================================================
/// 描画前処理 (SwapChain用)
///==========================================================
void DirectXCommon::PreDraw() {
    // これから書き込むバックバッファのインデックスを取得
    backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

    // 現在の状態がすでにPIXEL_SHADER_RESOURCEでない場合のみ遷移
    if (renderTextureCurrentState_ != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
        PutTransitionBarrier(renderTextureResource_.Get(),
            renderTextureCurrentState_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        renderTextureCurrentState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }

    /*PutTransitionBarrier(renderTextureResource_.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);*/

    // SwapChain をレンダーターゲットとして使用するためのバリア
    PutTransitionBarrier(swapChainResources_[backBufferIndex_].Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // 描画先のRTVとDSVを設定する (SwapChain)
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(backBufferIndex_);
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(); // 深度ステンシルビュー

    commandList_->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    // レンダーターゲットと深度ステンシルビューをクリア
    commandList_->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);
    // ビューポートとシザー矩形を設定
    commandList_->RSSetViewports(1, &viewport_);
    commandList_->RSSetScissorRects(1, &scissorRect_);
}
///==========================================================
/// 描画後処理
///==========================================================
// RenderTexture に描画する処理が終わった後、シェーダーリソースとして使う前に状態を遷移させる。
void DirectXCommon::DepthBarrierTransition() {

    // PutTransitionBarrier(depthStencilResource_.Get(),
    //	 D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DirectXCommon::PostDraw() {

    // TransitionBarrierを張る
    PutTransitionBarrier(swapChainResources_[backBufferIndex_].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    hr_ = commandList_->Close();
    assert(SUCCEEDED(hr_));

    // GPUにコマンドリストの実行を行わせる
    Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = {commandList_.Get()};
    commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

    // GPUとOSに画面の交換を行うよう通知する
    swapChain_->Present(1, 0);
    // Fenceの値を更新
    fenceValue_++;
    // GPUがここまでたどりついた時に、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < fenceValue_) {
        // 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fenceEvent_ = CreateEvent(nullptr, false, false, nullptr);
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
        CloseHandle(fenceEvent_);
    }

    Frame::Update();

    // 次のフレーム用のコマンドリストを準備
    hr_ = commandAllocator_->Reset();
    assert(SUCCEEDED(hr_));
    hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr_));
}
///==========================================================
/// Command Excution
///==========================================================
void DirectXCommon::commandExecution(Microsoft::WRL::ComPtr<ID3D12Resource>& intermediateResource) {

    // intermediateResource = TextureManager::GetInstance()->UploadTextureDate(textureManager_->GetTextureResource(), textureManager_->GetMipImages(), device_.Get(), commandList_);

    hr_ = commandList_->Close();
    assert(SUCCEEDED(hr_));
    // GPUにコマンドリストの実行を行わせる
    Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = {commandList_.Get()};
    commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

    // Fenceの値を更新
    fenceValue_++;
    // GPUがここまでたどりついた時に、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < fenceValue_) {

        // 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
    // 次のフレーム用のコマンドリストを準備
    hr_ = commandAllocator_->Reset();
    assert(SUCCEEDED(hr_));
    hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr_));

    intermediateResource.Reset();
}

///==========================================================
/// 　オブジェクト解放
///==========================================================
void DirectXCommon::Finalize() {
    device_.Reset();
    commandAllocator_.Reset();
    CloseWindow(winApp_->GetHwnd());
}

//*************************************************************************************************************************
// 関数----------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************************

void DirectXCommon::ClearDepthBuffer() {

    // 描画先のRTVとDSVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(backBufferIndex_);
    commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
    // 指定した深度で画面全体をクリアする
    commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

///==========================================================
/// ディスクリプタヒープの生成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::InitializeDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.Type           = heapType; // レンダーターゲットビュー用
    DescriptorHeapDesc.NumDescriptors = numDescriptors; // ダブルバッファ用に2つ。
    DescriptorHeapDesc.Flags          = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr                        = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    // ディスクリプタヒープが作れなかったので起動出来ない
    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create descriptor heap.\n");

        return nullptr;
    }
    return descriptorHeap.Get();
}

///==========================================================
/// バッファリソース生成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {
    HRESULT hr;
    // 頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
    // 頂点リソース用設定
    D3D12_RESOURCE_DESC vertexResourceDesc{};
    // ばっぱリソース。テクスチャの場合はまた別の設定をする
    vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexResourceDesc.Width     = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点文
    // バッファの場合はこれらは1にする決まり
    vertexResourceDesc.Height           = 1;
    vertexResourceDesc.DepthOrArraySize = 1;
    vertexResourceDesc.MipLevels        = 1;
    vertexResourceDesc.SampleDesc.Count = 1;
    // バッファの場合これにする決まり
    vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // 実際に頂点リソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> result = nullptr;

    hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&result));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create bufferResource heap.\n");
        // Releaseビルドでも descriptorHeap が null で返るようにする
        return nullptr;
    }

    return result.Get();
}

///==========================================================
/// 生成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width            = width; // textureの幅
    resourceDesc.Height           = height; // textureの高さ
    resourceDesc.MipLevels        = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行きor配列Textureの配列数
    resourceDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStenilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
    resourceDesc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
    depthClearValue.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

    // Resoureceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
        &depthClearValue, // Clear最適値
        IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create depth senthil resource heap.\n");
        // Releaseビルドでも descriptorHeap が null で返るようにする
        return nullptr;
    }
    return resource.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

void DirectXCommon::PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After) {
    // 今回のバリアはTransition
    barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // Noneにしておく
    barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    // バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier_.Transition.pResource = pResource;
    // 遷移前(現在)のResourceState
    barrier_.Transition.StateBefore = Before;
    // 遷移後のResourceState
    barrier_.Transition.StateAfter = After;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier_);
}

/// RebderTextureの作成
Microsoft::WRL::ComPtr<ID3D12Resource>
DirectXCommon::CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height,
    DXGI_FORMAT format, const Vector4& clearColor) {
    // 1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    resourceDesc.Width              = width;
    resourceDesc.Height             = height;
    resourceDesc.DepthOrArraySize   = 1; // 必須
    resourceDesc.MipLevels          = 1; // 最低1は必要
    resourceDesc.Format             = format;
    resourceDesc.SampleDesc.Count   = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    clearValue_.Format   = format;
    clearValue_.Color[0] = clearColor.x;
    clearValue_.Color[1] = clearColor.y;
    clearValue_.Color[2] = clearColor.z;
    clearValue_.Color[3] = clearColor.w;

    // 2. 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 3. Resourceを生成してreturnする
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue_,
        IID_PPV_ARGS(&resource));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to createRender Texture Resource heap.\n");
        // Releaseビルドでも descriptorHeap が null で返るようにする
        return nullptr;
    }
    return resource;
}

void DirectXCommon::CreateRnderSrvHandle() {

    uint32_t srvIndex = srvManager_->Allocate();

    renderTextureGPUSrvHandle_ = srvManager_->GetGPUDescriptorHandle(srvIndex);
    renderTextureCPUSrvHandle_ = srvManager_->GetCPUDescriptorHandle(srvIndex);

    // SRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Texture2D.MipLevels     = 1;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    /// Srvの生成
    srvManager_->CreateSRVforTexture2D(
        srvIndex,
        renderTextureResource_.Get(),
        srvDesc);
}