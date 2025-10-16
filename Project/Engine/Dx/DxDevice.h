#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

class DxDevice {
public:
    DxDevice()  = default;
    ~DxDevice() = default;

    // 初期化
    void Init();
    // 終了処理
    void Finalize();

private:

    // DXGIファクトリーとデバイス初期化
    void CreateFactory();
    void CreateDevice();
    void SetupDebugLayer();

    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    HRESULT hr_;

public:
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
    Microsoft::WRL::ComPtr<IDXGIFactory7> GetFactory() const { return dxgiFactory_; }
};