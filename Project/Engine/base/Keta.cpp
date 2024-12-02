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
	std::unique_ptr<SrvManager> sSrvManager = nullptr;

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
	sSrvManager = std::make_unique<SrvManager>();
	sSrvManager->Init(sDirectXCommon);

	/// Object3dCommon
	sObject3DCommon = Object3DCommon::GetInstance();
	sObject3DCommon->Init(sDirectXCommon);

	///ParticleCommon
	sParticleCommon = ParticleCommon::GetInstance();
	sParticleCommon->Init(sDirectXCommon);

	ParticleManager::GetInstance()->Init(sSrvManager.get());

	/// SpriteCommon
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Init(sDirectXCommon);

	/// ModelManager
	sModelManager = ModelManager::GetInstance();
	sModelManager->Initialize(sDirectXCommon);

	/// ImguiManager
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Init(sWinApp, sDirectXCommon,sSrvManager.get());

	/// TextureManager
	stextureManager = TextureManager::GetInstance();
	stextureManager->Init(sDirectXCommon,sSrvManager.get());

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
	imguiManager->Begin();	
	//開発者UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に書き換える	
#endif
	input->Update();
	sDirectXCommon->PreDraw();
	sSrvManager->PreDraw();
}
//フレームの終わり
void Keta::EndFrame() {
#ifdef _DEBUG
	ImGui::Render();
#endif
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