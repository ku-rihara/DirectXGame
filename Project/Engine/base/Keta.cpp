#include "Keta.h"
//class
#include"2d/ImGuiManager.h"
#include"TextureManager.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"audio/Audio.h"
#include"Object3DCommon.h"
#include"SpriteCommon.h"
#include"3d/ModelManager.h"
#include"input/Input.h"
#include"SrvManager.h"
#include"utility/Particle/ParticleCommon.h"
#include"utility/Particle/ParticleManager.h"

#include<imgui_impl_dx12.h>

#include"Matrix4x4.h"
#include"function/Convert.h"
#include <string>

namespace {

	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* imguiManager = nullptr;
	TextureManager* stextureManager = nullptr;
	SpriteCommon* spriteCommon = nullptr;
	Object3DCommon* sObject3DCommon = nullptr;
	ModelManager* sModelManager = nullptr;
	ParticleCommon*sParticleCommon = nullptr;
	SrvManager* sSrvManager = nullptr;

	Audio* audio = nullptr;
	Input* input = nullptr;
}


void Keta::Initialize(const char* title, int width, int height) {
	
	//ゲームウィンドウの作成
	std::string windowTitle = std::string(title);
	auto&& titleString= ConvertString(windowTitle);
	sWinApp =new WinApp();
	sWinApp->MakeWindow(titleString.c_str(), width, height);

	//DirectX初期化
	sDirectXCommon = DirectXCommon::GetInstance();
	sDirectXCommon->Init(sWinApp, width, height);
	sDirectXCommon->CreateGraphicPipelene();

	/// srvManager
	sSrvManager = SrvManager::GetInstance();
	sSrvManager->Init(sDirectXCommon);

	/// Object3dCommon
	sObject3DCommon = Object3DCommon::GetInstance();
	sObject3DCommon->Init(sDirectXCommon);

	///ParticleCommon
	sParticleCommon = ParticleCommon::GetInstance();
	sParticleCommon->Init(sDirectXCommon);

	ParticleManager::GetInstance()->Init(sSrvManager);

	/// SpriteCommon
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Init(sDirectXCommon);

	/// ModelManager
	sModelManager = ModelManager::GetInstance();
	sModelManager->Initialize(sDirectXCommon);

	/// ImguiManager
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Init(sWinApp, sDirectXCommon,sSrvManager);

	/// TextureManager
	stextureManager = TextureManager::GetInstance();
	stextureManager->Init(sDirectXCommon,sSrvManager);

	/// Input
	input = Input::GetInstance();
	input->Init(sWinApp->GetHInstaice(), sWinApp->GetHwnd());

	/// Audio
	audio = Audio::GetInstance();
	audio->Init();
}

//メッセージがなければループする
int Keta::ProcessMessage() {
	return sWinApp->ProcessMessage();
}

//フレームの始め
void Keta::BeginFrame() {
#ifdef _DEBUG
	imguiManager->Begin();	/// imGui begin
#endif
	input->Update();	
}

//フレームの始め
void Keta::PreDraw() {
	sDirectXCommon->PreDraw();
	sSrvManager->PreDraw();
}

//フレームの終わり
void Keta::EndFrame() {

#ifdef _DEBUG
	ImGui::Render();
#endif

	//imguiManager->Draw();
	sDirectXCommon->PostDraw();
}

void Keta::Finalize() {
	CoUninitialize();
	audio->Finalize();
	stextureManager->Finalize();
	sDirectXCommon->ReleaseObject();
	sModelManager->Finalize();
	
#ifdef _DEBUG
	imguiManager->Finalizer();
#endif
	delete sWinApp;
}
//
//void Keta::UpdateMatrixAll() {
//	WorldTransformManager::GetInstance().UpdateAll();
//}