#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<imgui_impl_dx12.h>
#include<imgui_impl_win32.h>


class WinApp;
class DirectXCommon;
class SrvManager;
class ImGuiManager{
private:
	/// other class
	DirectXCommon* dxCommon_;
	SrvManager* pSrvManager_;
	
public://メンバ関数
	static ImGuiManager* GetInstance();
	//初期化
	void Init(WinApp* winApp, DirectXCommon* dxCommon,SrvManager*srvManager);
	//フレーム始め
	void Begin();
	//フレーム終わり
	void End();
	//終了
	void Finalizer();

	
};

