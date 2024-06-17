#include"Keta.h"
#include"Transform.h"
#include"WinApp.h"
#include "D3DResourceLeakCheck.h"
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"

#include "externals/imgui/imgui.h"


const char kWindowTitle[] = "CG2";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3DResourceLeakChecker leakCheck;
	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);

	Sprite* sprite=Sprite::GetInstance();
	Model* model = Model::GetInstance();

	Transform tramsform;
	Transform transformSprite;
	Transform cameraTransform;
	Transform uvTransformSprite;
	
	uvTransformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	tramsform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	transformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-5.0f} };

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
		
#ifdef _DEBUG
		ImGui::Begin("Window");

		if (ImGui::TreeNode("Camera")) {
			ImGui::DragFloat3("Scale", &cameraTransform.scale.x, 0.01f);
			ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Model")) {
			ImGui::DragFloat3("Scale", &tramsform.scale.x, 0.01f);
			ImGui::DragFloat3("Rotate", &tramsform.rotate.x, 0.01f);
			ImGui::DragFloat3("Translate", &tramsform.translate.x, 0.01f);
			ImGui::TreePop();
		}	
		if (ImGui::TreeNode("Sprite")) {
			ImGui::DragFloat3("Scale", &transformSprite.scale.x, 0.1f);
			ImGui::DragFloat3("Rotate", &transformSprite.rotate.x, 1.0f);
			ImGui::DragFloat3("Translate", &transformSprite.translate.x, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("UVTransform")) {
			ImGui::DragFloat2("Scale", &uvTransformSprite.scale.x, 0.1f,-10.0f,10.0f);
			ImGui::DragFloat2("Translate", &uvTransformSprite.translate.x, 0.01f,-10.0f,10.0f);
			ImGui::SliderAngle("Rotate", &uvTransformSprite.rotate.z);
			ImGui::TreePop();
		}
		ImGui::End();
#endif
		//tramsform.rotate.y += 0.03f;
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.0f);

		//三角形
		Matrix4x4 worldMatrix = MakeAffineMatrix(tramsform.scale, tramsform.rotate, tramsform.translate);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		
		//スプライト
		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		Matrix4x4 projectionMatrixSprite=MakeOrthographicMatrix(0.0f,0.0f, float(WinApp::kWindowWidth),float(WinApp::kWindowHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, projectionMatrixSprite);
		model->SetwvpDate(worldViewProjectionMatrix);
		sprite->SetTransformationMatrixDataSprite(worldViewProjectionMatrixSprite);

		//UVTransform
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
		sprite->SetUVTransformSprite(uvTransformMatrix);
		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	return 0;
}

