#pragma once
#include<d3d12.h>
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"

class WinApp;
class DirectXCommon;

class ImGuiManager{
private://メンバ変数
	DirectXCommon* dxCommon_;
	ID3D12DescriptorHeap* srvDescriptorHeap_;

public://メンバ関数
	static ImGuiManager* GetInstance();
	//初期化
	void Init(WinApp* winApp, DirectXCommon* dxCommon);
	//フレーム始め
	void Begin();
	//フレーム終わり
	void End();
	//終了
	void Finalizer();

	ID3D12DescriptorHeap* GetSrvDescriptorHeap()const { return srvDescriptorHeap_; }
	void SetSrvDescriptorHeap(ID3D12DescriptorHeap* srv) { this->srvDescriptorHeap_=srv; }
};

