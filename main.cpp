#include"Keta.h"
//Transform
#include "WorldTransform.h"
#include"ViewProjection.h"
//DirectX
#include"WinApp.h"
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"
#include "DirectXCommon.h"
#include"D3DResourceLeakCheck.h"
#include"TextureManager.h"
//etc
#include "Audio.h"
#include"Input.h"
#include"DebugCamera.h"
#include "externals/imgui/imgui.h"


const char kWindowTitle[] = "LE2A_11_クリハラ_ケイタ_CG3";

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	/*D3DResourceLeakChecker leakCheck;*/
	//ライブラリの初期化
	Keta::Initialize(kWindowTitle, 1280, 720);
	TextureManager* textureManager = TextureManager::GetInstance();
	/*textureManager->LoadTexture("Resources/default.png");*/

	Sprite* sprite = Sprite::GetInstance();
	int soundData = Audio::GetInstance()->SoundLoadWave("Resources/fanfare.wav");

	//モデル読み込み
	Model* modelPlane = Model::Create("Plane");
    uint32_t modelInstance = modelPlane->GetKnumInstance();
	sprite->CreateSprite();
	//描画フラグ
	bool isDrawPlane = true;
	
	/*Model*  modelSuzanne_ = Model::Create("suzanne");
	*//*Model* modelTeaPot_ = Model::Create("teapot");*/
	Model* modelFence_ = Model::Create("Fence");

	//// それぞれのテクスチャをロード
	/*uint32_t uvHandle= textureManager->LoadTexture("Resources/uvChecker.png");*/
	/*uint32_t SuzanneHandle = TextureManager::GetInstance()->LoadTextureResource(model_->GetModelData().material.textureFilePath);
	*///uint32_t teaPotHandle = TextureManager::GetInstance()->LoadTextureResource(modelTeaPot_->GetModelData().material.textureFilePath);
	//uint32_t FenceHandle = TextureManager::GetInstance()->LoadTextureResource(modelFence_->GetModelData().material.textureFilePath);
	ViewProjection viewProjection;
	//ワールドトランスフォーム宣言***********
	WorldTransform PlaneTransform;
	WorldTransform FenceTransform;
	WorldTransform SuzanneTransform;
	std::vector<WorldTransform>PlaneTransforms(modelInstance);
	//スプライト
	WorldTransform transformSprite;
	WorldTransform uvTransformSprite;
	//デバッグカメラ

	DebugCamera* debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->Init();
	//ワールドトランスフォーム初期化******************************
	PlaneTransform.Init();
	FenceTransform.Init();
	debugCamera_->Init();
	uvTransformSprite.Init();

	
	//PlaneParticle
	for (uint32_t index = 0; index < modelInstance; ++index) {
		PlaneTransforms[index].Init();
		PlaneTransforms[index].translation_ = { index * 0.1f,index * 0.1f ,index * 0.1f };
	}

	//ワールドトランスフォーム値セット****************************
	transformSprite.scale_.x = 0.7f;
	PlaneTransform.rotation_.y = -3.0f;
	
	//ウィンドウのxボタンが押されるまでループ
	while (Keta::ProcessMessage() == 0) {
		//フレームの開始
		Keta::BeginFrame();
		debugCamera_->Update();

		if (Input::GetInstance()->TrrigerKey(DIK_F)) {
			Audio::GetInstance()->SoundPlayWave(soundData);
		}
#ifdef _DEBUG
		ImGui::Begin("Window");

		if (ImGui::TreeNode("IsDrawModel")) {
			ImGui::Checkbox("isPlane", &isDrawPlane);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("WorldTransform")) {
			
			if (ImGui::TreeNode("Plane")) {
				ImGui::DragFloat3("Scale", &PlaneTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &PlaneTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &PlaneTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Fence")) {
				ImGui::DragFloat3("Scale", &FenceTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &FenceTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &FenceTransform.translation_.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Suzanne")) {
				ImGui::DragFloat3("Scale", &SuzanneTransform.scale_.x, 0.01f);
				ImGui::DragFloat3("Rotate", &SuzanneTransform.rotation_.x, 0.01f);
				ImGui::DragFloat3("Translate", &SuzanneTransform.translation_.x, 0.01f);
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
		ImGui::End();

#endif
		//ワールド行列更新
		PlaneTransform.UpdateMatrix();
		FenceTransform.UpdateMatrix();
		SuzanneTransform.UpdateMatrix();
		for (uint32_t i = 0; i < modelInstance; i++) {
			PlaneTransforms[i].UpdateMatrix();
		}
	
		transformSprite.UpdateMatrix();
		uvTransformSprite.UpdateMatrix();

		//tramsform.rotate.y += 0.03f;
		
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
			
		/*	modelPlane->DrawParticle(PlaneTransforms, viewProjection);*/
		
			modelPlane->Draw(PlaneTransform, viewProjection);
			modelFence_->Draw(FenceTransform, viewProjection);
			///*modelSuzanne_->Draw(SuzanneTransform, viewProjection);
		

			////スプライト描画
			//sprite->DrawSprite(textureManager->GetTextureHandle(uvHandle));
		}
		
		//フレームの終了
		Keta::EndFrame();
	}

	//ライブラリの終了
	Keta::Finalize();
	delete debugCamera_;
	return 0;

}

