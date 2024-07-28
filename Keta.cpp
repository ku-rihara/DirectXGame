#include "Keta.h"
//class
#include"ImGuiManager.h"
#include"TextureManager.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "Audio.h"
#include"Input.h"

#include"Matrix4x4.h"
#include"Convert.h"
#include <string>

namespace {
	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* imguiManager = nullptr;
	TextureManager* textureManager = nullptr;
	Audio* audio = nullptr;
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

	HINSTANCE w;
	input = Input::GetInstance();
	input->Init(w,sWinApp->GetHwnd());

	audio = Audio::GetInstance();
	audio->Init();

	textureManager = TextureManager::GetInstance();
	textureManager->Load();
	sDirectXCommon->commandExecution();
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
	audio->Finalizer();
	sDirectXCommon->ReleaseObject();
	
#ifdef _DEBUG
	imguiManager->Finalizer();
#endif
}


int Keta::SoundLoadWave(const char* filename) {
	return audio->SoundLoadWave(filename);
}


void Keta::SoundPlayWave(int soundId) {
	audio->SoundPlayWave(soundId);
}