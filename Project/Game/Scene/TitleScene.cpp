///scene
#include"Manager/SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"

#include "base/TextureManager.h"
//class
#include"utility/Particle/ParticleManager.h"

//math
#include"Frame/Frame.h"
#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	BaseScene::Init();

	uint32_t t = TextureManager::GetInstance()->LoadTexture("Resources/Texture/boal.png");
	uint32_t t2 = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");

	smokeName_ = "EnemyDeathSmoke";
	smokeEmitter_.reset(ParticleEmitter::CreateParticle(smokeName_, "Plane", ".obj", 900));
	smokeEmitter_->SetTextureHandle(t);
	smokeEmitter_->SetBlendMode(ParticleCommon::BlendMode::None);

	fireSmokeName_ = "EnemyDeathFireSmoke";
	fireSmokeEmitter_.reset(ParticleEmitter::CreateParticle(fireSmokeName_, "Plane", ".obj", 900));
	fireSmokeEmitter_->SetTextureHandle(t2);
	fireSmokeEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);

	sparkName_ = "EnemyDeathSpark";
	sparkEmitter_.reset(ParticleEmitter::CreateParticle(sparkName_, "Plane", ".obj", 900));
	sparkEmitter_->SetTextureHandle(t2);
	sparkEmitter_->SetBlendMode(ParticleCommon::BlendMode::Add);
	ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void TitleScene::Update() {

	
	smokeEmitter_->Update();
	smokeEmitter_->EditorUpdate();
	smokeEmitter_->Emit();

	fireSmokeEmitter_->Update();
	fireSmokeEmitter_->EditorUpdate();
	fireSmokeEmitter_->Emit();

	sparkEmitter_->Update();
	sparkEmitter_->EditorUpdate();
	sparkEmitter_->Emit();

	ParticleManager::GetInstance()->Update();

	Debug();
	ViewProjectionUpdate();

	if (input_->TrrigerKey(DIK_RETURN)) {

		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

/// ===================================================
/// モデル描画
/// ===================================================
void TitleScene::ModelDraw() {
	/// commandList取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	Model::PreDraw(commandList);

	smokeEmitter_->DebugDraw(viewProjection_);
	smokeEmitter_->RailDraw(viewProjection_);
	ParticleManager::GetInstance()->Draw(viewProjection_);

}

   /// ===================================================
   /// パーティクル描画
   /// ===================================================
//void TitleScene::ParticleDraw() {
//	
//}

/// ===================================================
   /// スプライト描画
   /// ===================================================
void TitleScene::SpriteDraw() {

}

void TitleScene::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
	ImGui::End();
#endif
}


// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
	BaseScene::ViewProjectionUpdate();
}


void TitleScene::ViewProssess() {
	viewProjection_.UpdateMatrix();
}
