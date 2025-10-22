#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class WinApp;
class DirectXCommon;
class SrvManager;

/// <summary>
/// ImGuiの管理クラス
/// </summary>
class ImGuiManager {
public:
    ~ImGuiManager() = default;

    static ImGuiManager* GetInstance();

    ///===========================================================
    /// public method
    ///===========================================================

    // 初期化、描画
    void Init(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);
    void Draw();

    void Begin();     //< 開始
    void preDraw();   //< 終わり
    void Finalizer(); //< 解放
  

private:
    ///===========================================================
    /// private variants
    ///============================================================

    /// other class
    DirectXCommon* dxCommon_;
    SrvManager* pSrvManager_;
    // SRV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};
