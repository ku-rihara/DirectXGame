#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>
#include <wrl/client.h>

#pragma comment(lib, "dxcompiler.lib")

class DxCompiler {
public:
    DxCompiler()  = default;
    ~DxCompiler() = default;

    // 初期化
    void Init();

    // シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile);

    // 終了処理
    void Finalize();

private:
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_                = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_         = nullptr;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

};