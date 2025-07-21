#include "DxCompiler.h"
#include "function/Convert.h"
#include "function/Log.h"
#include <cassert>
#include <format>

void DxCompiler::Init() {
    // dxcCompilerを初期化
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(hr));

    // includeに対応するための設定
    hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> DxCompiler::CompileShader(
    const std::wstring& filePath,
    const wchar_t* profile) {

    // コンパイル開始ログ
    Log(ConvertString(std::format(L"Begin CompileShader,path:{}\n", filePath, profile)));

    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr                     = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    assert(SUCCEEDED(hr));

    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    // コンパイルオプション
    LPCWSTR arguments[] = {
        filePath.c_str(),
        L"-E",
        L"main",
        L"-T",
        profile,
        L"-Zi",
        L"-Qembed_debug",
        L"-Od",
        L"-Zpr",
    };

    // シェーダーをコンパイル
    Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
    hr                                              = dxcCompiler_->Compile(
        &shaderSourceBuffer,
        arguments,
        _countof(arguments),
        includeHandler_.Get(),
        IID_PPV_ARGS(&shaderResult));
    assert(SUCCEEDED(hr));

    // エラーチェック
    Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        assert(false);
    }

    // コンパイル結果を取得
    Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
    hr                                          = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.GetAddressOf()), nullptr);
    assert(SUCCEEDED(hr));

    Log(ConvertString(std::format(L"Compile Succeeded,path:{}\n", filePath, profile)));

    // リソース解放
    shaderSource->Release();

    return shaderBlob;
}

void DxCompiler::Finalize() {
   /* if (dxcCompiler_) {
        dxcCompiler_.Reset();
    }
    if (dxcUtils_) {
        dxcUtils_.Reset();
    }
    if (includeHandler_) {
        includeHandler_.Reset();
    }*/
}