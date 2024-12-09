#pragma once

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

	void Draw();

	
};

