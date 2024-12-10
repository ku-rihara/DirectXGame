#pragma once

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

public:
	static ImGuiManager* GetInstance();

	///===========================================================
   ///public method
   ///===========================================================
	
	void Init(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);//初期化

	void Begin();//開始

	void End();	//終わり
	
	void Finalizer();//解放

	/*void Draw();*/
};

