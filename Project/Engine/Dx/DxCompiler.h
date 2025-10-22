#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <string>
#include <wrl.h>
#include <wrl/client.h>

#pragma comment(lib, "dxcompiler.lib")

/// <summary>
/// シェーダーコンパイルを行うクラス
/// </summary>
class DxCompiler {
public:
    DxCompiler()  = default;
    ~DxCompiler() = default;

    void Init();     //< 初期化
    void Finalize(); //< 終了処理

    /// <summary>
    /// シェーダーコンパイル
    /// </summary>
    /// <param name="filePath">シェーダーファイルパス</param>
    /// <param name="profile">シェーダープロファイル</param>
    /// <returns>コンパイル済みシェーダーBlob</returns>
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile);

private:
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_                = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_         = nullptr;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
};