#include "Keta.h"
//class
#include"ImGuiManager.h"
#include"TextureManager.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "SoundManager.h"
#include"Input.h"
#include"WorldTransformManager.h"

#include"Matrix4x4.h"
#include"Convert.h"
#include <string>

namespace {
	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* imguiManager = nullptr;
	TextureManager* textureManager = nullptr;
	SoundManager* soundManager = nullptr;
	Input* input = nullptr;
}


void Keta::Initialize(const char* title, int width, int height) {
	
	//ゲームウィンドウの作成
	std::string windowTitle = std::string(title);
	auto&& titleString= ConvertString(windowTitle);
	sWinApp = WinApp::GetInstance();
	sWinApp->MakeWindow(titleString.c_str(), width, height);

	//DirectX初期化
	sDirectXCommon = DirectXCommon::GetInstance();
	sDirectXCommon->Init(sWinApp, width, height);
	sDirectXCommon->CreateGraphicPipelene();

	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Init(sWinApp, sDirectXCommon);

	input = Input::GetInstance();
	input->Init(sWinApp->GetHInstaice(), sWinApp->GetHwnd());

	soundManager = SoundManager::GetInstance();
	soundManager->Init();

	
}

//メッセージがなければループする
int Keta::ProcessMessage() {
	return sWinApp->ProcessMessage();
}
//フレームの始め
void Keta::BeginFrame() {
#ifdef _DEBUG
	imguiManager->Begin();	
	//開発者UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える	
#endif
	input->Update();
	sDirectXCommon->ScreenClear();
}
//フレームの終わり
void Keta::EndFrame() {
#ifdef _DEBUG
	ImGui::Render();
#endif
	sDirectXCommon->CommandKick();
}

void Keta::Finalize() {
	CoUninitialize();
	soundManager->Finalize();
	sDirectXCommon->ReleaseObject();
	
#ifdef _DEBUG
	imguiManager->Finalizer();
#endif
}

void Keta::UpdateMatrixAll() {
	WorldTransformManager::GetInstance().UpdateAll();
}