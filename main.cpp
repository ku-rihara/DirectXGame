#include"Keta.h"
#include"Transform.h"
#include "WorldTransform.h"
#include "WorldTransformManager.h"
#include"ViewProjection.h"
#include"WinApp.h"
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"
#include "DirectXCommon.h"
#include "SoundManager.h"
#include"Input.h"
#include"DebugCamera.h"
#include"TextureManager.h"

#include"D3DResourceLeakCheck.h"

#include "externals/imgui/imgui.h"


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG2";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	/*D3DResourceLeakChecker leakCheck;*/
	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);
	DirectXCommon* d = DirectXCommon::GetInstance();
	Sprite* sprite = Sprite::GetInstance();
	Model* modelPlane = Model::GetInstance();
	Model* modelAxis = Model::GetInstance();
	modelPlane->CreateModel("Plane.obj");
	modelAxis->CreateModel("axis.obj");
	TextureManager*te = TextureManager::GetInstance();
	te->Load();
	d->commandExecution();
	ViewProjection viewProjection;
	WorldTransform axisTransform_;
	WorldTransform tramsform;
	WorldTransform transformSprite;
	WorldTransform cameraTransform;
	WorldTransform uvTransformSprite;
	DebugCamera* debugCamera_ = new DebugCamera(1280, 720);
	axisTransform_.Init();
	debugCamera_->Init();
	uvTransformSprite.Init();
	tramsform.Init();
	transformSprite.Init();

	cameraTransform.translation_.z = -5.0f;
	cameraTransform.Init();

	int soundId = SoundManager::GetInstance()->SoundLoadWave("Resources/fanfare.wav");
	SoundManager::GetInstance()->SoundPlayWave(soundId);

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();


		XINPUT_STATE xState;

		bool xStateRetrieved = Input::GetInstance()->GetJoystickState(0, xState);
		debugCamera_->Update();

		if (xStateRetrieved) {
			// XInput のジョイスティック状態を使った処理
			if (xState.Gamepad.wButtons&XINPUT_GAMEPAD_A) {
				OutputDebugStringA("XInput: Joystick A \n");
			}
		}

		if (Input::GetInstance()->TrrigerKey(DIK_A)) {
			OutputDebugStringA("HIT A\n");
		}
#ifdef _DEBUG
		ImGui::Begin("Window");

		if (ImGui::TreeNode("Camera")) {
			ImGui::DragFloat3("Scale", &cameraTransform.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &cameraTransform.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &cameraTransform.translation_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Model")) {
			ImGui::DragFloat3("Scale", &tramsform.scale_.x, 0.01f);
			ImGui::DragFloat3("Rotate", &tramsform.rotation_.x, 0.01f);
			ImGui::DragFloat3("Translate", &tramsform.translation_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Sprite")) {
			ImGui::DragFloat3("Scale", &transformSprite.scale_.x, 0.1f);
			ImGui::DragFloat3("Rotate", &transformSprite.rotation_.x, 1.0f);
			ImGui::DragFloat3("Translate", &transformSprite.translation_.x, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("UVTransform")) {
			ImGui::DragFloat2("Scale", &uvTransformSprite.scale_.x, 0.1f, -10.0f, 10.0f);
			ImGui::DragFloat2("Translate", &uvTransformSprite.translation_.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("Rotate", &uvTransformSprite.rotation_.z);
			ImGui::TreePop();
		}
		ImGui::End();
#endif
		Keta::UpdateMatrixAll();
		//tramsform.rotate.y += 0.03f;
		/*Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale_, cameraTransform.rotation_, cameraTransform.translation_);*/
	/*	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.0f);*/

		// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
		viewProjection.matView_ = debugCamera_->GetViewProjection().matView_;
		viewProjection.matProjection_ = debugCamera_->GetViewProjection().matProjection_;

		

		//スプライト
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = transformSprite.matWorld_* projectionMatrixSprite;
	
		sprite->SetTransformationMatrixDataSprite(worldViewProjectionMatrixSprite);

		//UVTransform
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale_);
		uvTransformMatrix = (uvTransformMatrix* MakeRotateZMatrix(uvTransformSprite.rotation_.z));
		uvTransformMatrix = (uvTransformMatrix* MakeTranslateMatrix(uvTransformSprite.translation_));
		sprite->SetUVTransformSprite(uvTransformMatrix);
		
		//Draw********************************************************
		modelPlane->Draw(tramsform,viewProjection);
		modelAxis->Draw(axisTransform_, viewProjection);

		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	delete debugCamera_;
	return 0;

}

