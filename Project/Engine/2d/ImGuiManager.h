#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<imgui_impl_dx12.h>
#include<imgui_impl_win32.h>


class WinApp;
class DirectXCommon;

class ImGuiManager{
private://メンバ変数
	DirectXCommon* dxCommon_;
	
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

	
};

