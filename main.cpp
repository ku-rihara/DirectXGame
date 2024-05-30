#include"Keta.h"
#include"Transform.h"
#include"WinApp.h"
#include"DirectXCommon.h"

#include "externals/imgui/imgui.h"

const char kWindowTitle[] = "CG2";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);
	DirectXCommon* dxcommon;
	dxcommon = DirectXCommon::GetInstance();

	Transform tramsform[triangleNum];
	Matrix4x4 worldMatrix[triangleNum];
	Matrix4x4 worldViewProjectionMatrix[triangleNum];
	Transform cameraTransform;
	tramsform[0] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	tramsform[1] = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-5.0f} };

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
#ifdef _DEBUG
		ImGui::Begin("Transform");
		ImGui::DragFloat3("Scale", &tramsform[0].scale.x, 0.1f);
		ImGui::DragFloat3("Rotate", &tramsform[0].rotate.x, 0.1f);
		ImGui::DragFloat3("Translate", &tramsform[0].translate.x, 0.1f);
		ImGui::End();
		ImGui::Begin("Camera");
		ImGui::DragFloat3("Scale", &cameraTransform.scale.x, 0.1f);
		ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.1f);
		ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.1f);
		ImGui::End();
#endif
		//tramsform.rotate.y += 0.03f;
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.0f);

		for (int i = 0; i < triangleNum; i++) {
			//三角形
			worldMatrix[i] = MakeAffineMatrix(tramsform[i].scale, tramsform[i].rotate, tramsform[i].translate);
			worldViewProjectionMatrix[i] = Multiply(worldMatrix[i], Multiply(viewMatrix, projectionMatrix));
			dxcommon->SetwvpDate(i, worldViewProjectionMatrix[i]);
		}

		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	return 0;
}

