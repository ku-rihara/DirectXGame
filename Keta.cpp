#include "Keta.h"
//class
#include"ImGuiManager.h"
#include"TextureManager.h"

#include"Transform.h"
#include"Matrix4x4.h"

#include"Convert.h"
#include <string>

namespace {
	WinApp* sWinApp = nullptr;
	DirectXCommon* sDirectXCommon = nullptr;
	ImGuiManager* imguiManager = nullptr;
	TextureManager* textureManager = nullptr;

	Transform tramsform;
	Transform cameraTransform;
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

	textureManager = TextureManager::GetInstance();
	textureManager->Load();
	
	 tramsform={ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	 cameraTransform= { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-5.0f} };
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
	ImGui::ShowDemoWindow();
#endif
	tramsform.rotate.y += 0.03f;
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(sWinApp->kWindowWidth) / float(sWinApp->kWindowHeight), 0.1f, 100.0f);

	Matrix4x4 worldMatrix = MakeAffineMatrix(tramsform.scale, tramsform.rotate, tramsform.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	sDirectXCommon->SetwvpDate(worldViewProjectionMatrix);

	ImGui::Render();
	sDirectXCommon->ScreenClear();
}
//フレームの終わり
void Keta::EndFrame() {

	sDirectXCommon->CommandKick();
}

void Keta::Finalize() {
	CoUninitialize();
	sDirectXCommon->ReleaseObject();
#ifdef _DEBUG
	imguiManager->Finalizer();
#endif
	sDirectXCommon->ResourceLeakCheck();
}