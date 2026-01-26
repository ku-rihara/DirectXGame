#include "ImGuiManager.h"

using namespace KetaEngine;
#include "Base/Descriptors/SrvManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxSwapChain.h"
#include "Base/WinApp.h"

#include <d3d12.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

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
#ifdef _DEBUG
    // ImGuiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // レイアウトファイルのパス設定
    io.IniFilename = "imgui_layout.ini";

    // 矢印記号を含む範囲を追加
    ImFontConfig config;
    config.MergeMode = false;

    // 日本語 + 基本記号の範囲を指定
    static const ImWchar ranges[] = {
        0x0020,
        0x00FF, // Basic Latin + Latin Supplement
        0x2000,
        0x206F, // General Punctuation
        0x2190,
        0x21FF, // 矢印記号の範囲
        0x3000,
        0x30FF, // 日本語の句読点とひらがな
        0x3131,
        0x3163, // Korean alphabets
        0x4e00,
        0x9FAF, // CJK統合漢字
        0xFF00,
        0xFFEF, // 半角・全角形
        0,
    };

    io.Fonts->AddFontFromFileTTF("Resources/Font/MPLUS1p-Medium.ttf", 18.0f, &config, ranges);

    // ImGui用のSRVインデックスを確保
    int srvIndex = pSrvManager_->Allocate();

    // Darkテーマに設定
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp->GetHwnd());

    io.Fonts->Build();

    ImGui_ImplDX12_Init(
        dxCommon_->GetDevice().Get(),
        dxCommon_->GetDxSwapChain()->GetDesc().BufferCount,
        dxCommon_->GetDxSwapChain()->GetRTVDesc().Format,
        pSrvManager_->GetDescriptorHeap(),
        pSrvManager_->GetCPUDescriptorHandle(srvIndex),
        pSrvManager_->GetGPUDescriptorHandle(srvIndex));


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

    // ドッキングスペースを作成
    SetupDockSpace();
#endif
}

///===========================================================
/// ドッキングスペース設定
///===========================================================
void ImGuiManager::SetupDockSpace() {
#ifdef _DEBUG
    // フルスクリーンのドッキングスペースを作成
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // フルスクリーンモードの場合は背景を透明にする
    if (isFullScreenMode_) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // メニューバー
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Window")) {
            ImGui::Separator();
            ImGui::Checkbox("フルスクリーンモード", &isFullScreenMode_);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // ドッキングスペースを有効化
    ImGuiID dockSpace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockSpace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
#endif
}

///===========================================================
/// 終わり
///===========================================================
void ImGuiManager::preDraw() {
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

void ImGuiManager::Draw() {
#ifdef _DEBUG
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // デスクリプタヒープの配列をセットするコマンド
    ID3D12DescriptorHeap* ppHeaps[] = {pSrvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    // 描画コマンドを発行
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}
