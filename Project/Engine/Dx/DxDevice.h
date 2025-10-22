#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// デバイスを管理するクラス
/// </summary>
class DxDevice {
public:
    DxDevice()  = default;
    ~DxDevice() = default;
 
    void Init();    //< 初期化
    void Finalize();//< 終了処理

private:
   
    void CreateFactory(); //< DXGIファクトリー作成
    void CreateDevice(); //< デバイス作成
    void SetupDebugLayer();//< デバッグレイヤー設定

    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    HRESULT hr_;

public:
    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
    Microsoft::WRL::ComPtr<IDXGIFactory7> GetFactory() const { return dxgiFactory_; }
};