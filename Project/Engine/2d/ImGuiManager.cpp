#include "2d/ImGuiManager.h"
#include "base/SrvManager.h"
#include "base/WinApp.h"
#include "Dx/DirectXCommon.h"

#include <d3d12.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

ImGuiManager* ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return &instance;
}

///===========================================================
/// 初期化
///===========================================================
void ImGuiManager::Init(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager) {
    winApp;
    dxCommon_    = dxCommon;
    pSrvManager_ = srvManager;

     //pSrvManager_->Allocate();//確保
    // dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, SrvManager::kMaxCount, true);
    // srvDescriptorHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, DirectXCommon::kMaxSRVCount, true);

#ifdef _DEBUG

    HRESULT result;
    // ImGuiの初期化

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキングを有効化

    // デスクリプタヒープ設定
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors             = 1;
    desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    // デスクリプタヒープ生成
    result = dxCommon_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
    assert(SUCCEEDED(result));

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp->GetHwnd());
    ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
        dxCommon_->GetSwapChainDesc().BufferCount, dxCommon_->GetRtvDesc().Format,
        srvHeap_.Get(),
        srvHeap_->GetCPUDescriptorHandleForHeapStart(),
        srvHeap_->GetGPUDescriptorHandleForHeapStart());
#endif
}

///===========================================================
/// 開始
///===========================================================
void ImGuiManager::Begin() {
#ifdef _DEBUG
    // ImGuiフレーム開始
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

#endif
}

///===========================================================
/// 終わり
///===========================================================
void ImGuiManager::preDrawa() {
#ifdef _DEBUG

    ImGui::Render();
#endif
}

///===========================================================
/// 解放処理
///===========================================================
void ImGuiManager::Finalizer() {
#ifdef _DEBUG
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

#endif
}

//
void ImGuiManager::Draw() {

#ifdef _DEBUG
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // デスクリプタヒープの配列をセットするコマンド
    ID3D12DescriptorHeap* ppHeaps[] = {srvHeap_.Get()};
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    // 描画コマンドを発行
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}
