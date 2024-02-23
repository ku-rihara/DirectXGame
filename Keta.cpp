#include "Keta.h"
#include"Convert.h"
#include <string>

namespace {
	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
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
}

int Keta::ProcessMessage() {
	return sWinApp->ProcessMessage();
}

void Keta::BeginFrame() {
	sDirectXCommon->ScreenClear();
}

void Keta::EndFrame() {
	sDirectXCommon->CommandKick();
}

void Keta::Finalize() {

	sDirectXCommon->ReleaseObject();
	sDirectXCommon->ResourceLeakCheck();
}