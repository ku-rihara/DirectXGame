#include"Keta.h"
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
	TextureManager* textureManager = TextureManager::GetInstance();

	textureManager->Load();
	Sprite* sprite = Sprite::GetInstance();
	Mesh* modelSphere = Mesh::GetInstance();
	//モデル読み込み
	Model* modelPlane = Model::Create("plane");
	Model* modeBunny = Model::Create("bunny");
	Model* modelMultiMesh = Model::Create("multiMesh");
	sprite->CreateSprite();
	modelSphere->CreateSphere();
	//描画フラグ
	bool isDrawSuzanne = false;
	bool isDrawPlane = true;
	bool isDrawSphere = false;
	bool isDrawTeaPot = false;
	bool isDrawBunny = false;
	bool isMultiMesh = false;
	bool isDrawFence = false;
	ViewProjection viewProjection;
	//ワールドトランスフォーム宣言***********
	WorldTransform PlaneTransform;
	WorldTransform bunnyTransform;
	WorldTransform suzanneTransform;
	WorldTransform transformSprite;
	WorldTransform uvTransformSprite;
	WorldTransform SphereWorldTransform;
	WorldTransform teaPotWorldTransform;
	WorldTransform multiMeshWorldTransform;
	WorldTransform fenceWorldTransform;
	//デバッグカメラ

	DebugCamera* debugCamera_ = new DebugCamera(1280, 720);

	//ワールドトランスフォーム初期化******************************
	PlaneTransform.Init();
	debugCamera_->Init();
	uvTransformSprite.Init();
	suzanneTransform.Init();
	transformSprite.Init();
	SphereWorldTransform.Init();
	teaPotWorldTransform.Init();
	bunnyTransform.Init();
	multiMeshWorldTransform.Init();
	fenceWorldTransform.Init();

	//ワールドトランスフォーム値セット****************************
	transformSprite.scale_.x = 0.7f;
	fenceWorldTransform.rotation_.y = -3.0f;
	PlaneTransform.rotation_.y = -3.0f;
	multiMeshWorldTransform.rotation_.y = -3.0f;
	int soundId = SoundManager::GetInstance()->SoundLoadWave("Resources/fanfare.wav");
	//SoundManager::GetInstance()->SoundPlayWave(soundId);

	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();


		XINPUT_STATE xState;
		XINPUT_STATE xStatePre;

		bool isState = Input::GetInstance()->GetJoystickState(0, xState);
		bool IsStatePre = Input::GetInstance()->GetJoystickStatePrevious(0, xStatePre);
		debugCamera_->Update();

		if (isState && IsStatePre) {
			// XInput のジョイスティック状態を使った処理
			if (xState.Gamepad.wButtons & XINPUT_GAMEPAD_A && xStatePre.Gamepad.wButtons != XINPUT_GAMEPAD_A) {
				SoundManager::GetInstance()->SoundPlayWave(soundId);
			}
		}

		if (Input::GetInstance()->TrrigerKey(DIK_A)) {
			SoundManager::GetInstance()->SoundPlayWave(soundId);
		}
#ifdef _DEBUG
		ImGui::Begin("Window");

		if (ImGui::TreeNode("IsDrawModel")) {
			ImGui::Checkbox("isPlane", &isDrawPlane);
			ImGui::Checkbox("isSuzanne", &isDrawSuzanne);
			ImGui::Checkbox("isSphere", &isDrawSphere);
			ImGui::Checkbox("isTeapot", &isDrawTeaPot);
			ImGui::Checkbox("isBunny", &isDrawBunny);
			ImGui::Checkbox("isMultiMesh", &isMultiMesh);
			ImGui::Checkbox("isFence", &isDrawFence);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("WorldTransform")) {
			if (ImGui::TreeNode("Suzanne")) {
				ImGui::DragFloat3("Scale", &suzanneTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &suzanneTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &suzanneTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Plane")) {
				ImGui::DragFloat3("Scale", &PlaneTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &PlaneTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &PlaneTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Sphere")) {
				ImGui::DragFloat3("Scale", &SphereWorldTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &SphereWorldTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &SphereWorldTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("teapot")) {
				ImGui::DragFloat3("Scale", &teaPotWorldTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &teaPotWorldTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &teaPotWorldTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("bunny")) {
				ImGui::DragFloat3("Scale", &bunnyTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &bunnyTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &bunnyTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("MultiMesh")) {
				ImGui::DragFloat3("Scale", &multiMeshWorldTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &multiMeshWorldTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &multiMeshWorldTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Fence")) {
				ImGui::DragFloat3("Scale", &fenceWorldTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &fenceWorldTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &fenceWorldTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Sprite")) {
				ImGui::DragFloat3("Scale", &transformSprite.scale_.x, 0.1f);
				ImGui::DragFloat3("Rotate", &transformSprite.rotation_.x, 1.0f);
				ImGui::DragFloat3("Translate", &transformSprite.translation_.x, 1.0f);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("UVTransform")) {
			ImGui::DragFloat2("Scale", &uvTransformSprite.scale_.x, 0.1f, -10.0f, 10.0f);
			ImGui::DragFloat2("Translate", &uvTransformSprite.translation_.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("Rotate", &uvTransformSprite.rotation_.z);
			ImGui::TreePop();
		}
		ImGui::End();
		//ライティング
		ImGui::Begin("Lighting");
		if (ImGui::TreeNode("Plane")) {
			modelPlane->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Suzanne")) {
			Model::GetInstance("suzanne")->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Sphere")) {
			modelSphere->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("teapot")) {
			Model::GetInstance("teapot")->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("bunny")) {
			modeBunny->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("MultiMesh")) {
			modelMultiMesh->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Fence")) {
			Model::GetInstance("Fence")->DebugImGui();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Sprite")) {
			sprite->DebugImGui();
			ImGui::TreePop();
		}
		ImGui::End();

#endif
		//全行列更新********************
		Keta::UpdateMatrixAll();
		//****************************
		//tramsform.rotate.y += 0.03f;
		/*Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale_, cameraTransform.rotation_, cameraTransform.translation_);*/
	/*	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight), 0.1f, 100.0f);*/

		// カメラ行列の計算をデバッグカメラのビュープロジェクションから行う
		viewProjection.matView_ = debugCamera_->GetViewProjection().matView_;
		viewProjection.matProjection_ = debugCamera_->GetViewProjection().matProjection_;

		//スプライト
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kWindowWidth), float(WinApp::kWindowHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = transformSprite.matWorld_ * projectionMatrixSprite;
		sprite->SetTransformationMatrixDataSprite(worldViewProjectionMatrixSprite);

		//UVTransform
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale_);
		uvTransformMatrix = (uvTransformMatrix * MakeRotateZMatrix(uvTransformSprite.rotation_.z));
		uvTransformMatrix = (uvTransformMatrix * MakeTranslateMatrix(uvTransformSprite.translation_));
		sprite->SetUVTransformSprite(uvTransformMatrix);

		//Draw********************************************************

		//平面描画
		if (isDrawPlane) {
			modelPlane->Draw(PlaneTransform, viewProjection, textureManager->GetTextureSrvHandleGPU());
			//スプライト描画
			sprite->DrawSprite();
		}
		//球
		if (isDrawSphere) {
			modelSphere->DrawSphere(SphereWorldTransform, viewProjection, textureManager->GetTextureSrvHandleGPU());
			//スプライト描画
			sprite->DrawSprite();
		}
		//ティーポット
		if (isDrawTeaPot) {
			Model::GetInstance("teapot")->Draw(teaPotWorldTransform, viewProjection, textureManager->GetTextureSrvHandleGPU3());
			//スプライト描画
			sprite->DrawSprite();
		}
		//バニー
		if (isDrawBunny) {
			modeBunny->Draw(bunnyTransform, viewProjection, textureManager->GetTextureSrvHandleGPU());
			//スプライト描画
			sprite->DrawSprite();
		}
		//マルチメッシュ
		if (isMultiMesh) {
			modelMultiMesh->Draw(multiMeshWorldTransform, viewProjection, textureManager->GetTextureSrvHandleGPU());
			//スプライト描画
			sprite->DrawSprite();
		}
		//フェンス
		if (isDrawFence) {
			Model::GetInstance("Fence")->Draw(fenceWorldTransform, viewProjection, textureManager->GetTextureSrvHandleGPU4());
			//スプライト描画
			sprite->DrawSprite();
		}
		//スザンヌ
		if (isDrawSuzanne) {
			Model::GetInstance("suzanne")->Draw(suzanneTransform, viewProjection, textureManager->GetTextureSrvHandleGPU2());
			//スプライト描画
			sprite->DrawSprite();
		}
		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	delete debugCamera_;
	return 0;

}

