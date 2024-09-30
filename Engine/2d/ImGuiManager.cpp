#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"


ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Init(WinApp* winApp, DirectXCommon* dxCommon) {
	winApp;
	dxCommon_ = dxCommon;
	srvDescriptorHeap_ = dxCommon_->CreateDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, DirectXCommon::kMaxSRVCount, true);

#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(), dxCommon_->GetSwapChainDesc().BufferCount, dxCommon_->GetRtvDesc().Format,
		srvDescriptorHeap_.Get(), srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
#endif
}

//フレーム始め
void ImGuiManager::Begin() {
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}
//フレーム終わり
void ImGuiManager::End() {
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render();
#endif
}

void ImGuiManager::Finalizer() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif
}