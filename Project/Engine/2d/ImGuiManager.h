#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace KetaEngine {

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
    /// private method
    ///===========================================================
    void SetupDockSpace(); //< ドッキングスペース設定

    ///===========================================================
    /// private variants
    ///============================================================

    /// other class
    DirectXCommon* dxCommon_;
    SrvManager* pSrvManager_;

    bool isFullScreenMode_ = false;

public:
    // フルスクリーンモード
    bool IsFullScreenMode() const { return isFullScreenMode_; }
    void SetFullScreenMode(bool enable) { isFullScreenMode_ = enable; }
};

}; // KetaEngine