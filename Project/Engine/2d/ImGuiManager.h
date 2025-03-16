#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class WinApp;
class DirectXCommon;
class SrvManager;
class ImGuiManager {
private:

	///===========================================================
   ///private variants
   ///============================================================

	/// other class
	DirectXCommon* dxCommon_;
	SrvManager* pSrvManager_;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>srvHeap_;
public:
	static ImGuiManager* GetInstance();

	///===========================================================
   ///public method
   ///===========================================================
	
	void Init(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);//初期化

	void Begin();//開始

	void preDrawa();	//終わり
	
	void Finalizer();//解放

	void Draw();
};

