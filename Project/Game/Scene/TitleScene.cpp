///scene
#include "TitleScene.h"
#include"Manager/SceneManager.h"
#include"utility/Particle/ParticleManager.h"


#include "base/TextureManager.h"
//class

//math

#include<imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {

}

void TitleScene::Init() {

	BaseScene::Init();

	damageName_ = "DamageParticle";
	damageEmitter_.reset(ParticleEmitter::CreateParticle(damageName_, "Plane", ".obj", 200, false));
	uint32_t handle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/circle.png");
	damageEmitter_->SetTextureHandle(handle);
	
}

void TitleScene::Update() {
	damageEmitter_->Update();
	damageEmitter_->EditorUpdate();
	

	damageEmitter_->Emit();
	
	ParticleManager::GetInstance()->Update(&viewProjection_);

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

	
}

/// ===================================================
   /// パーティクル描画
   /// ===================================================
void TitleScene::ParticleDraw() {

}

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