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

	emitter_.reset(ParticleEmitter::CreateParticle("punchEffect", "Plane", ".obj", 900));
	uint32_t t=TextureManager::GetInstance()->LoadTexture("Resources/Texture/default.png");
	emitter_->SetTextureHandle(t);

	ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);

}

void TitleScene::Update() {

	
	emitter_->Update();
	emitter_->EditorUpdate();
	emitter_->Emit();

	ParticleManager::GetInstance()->Update(viewProjection_);

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

	emitter_->DebugDraw(viewProjection_);
	emitter_->RailDraw(viewProjection_);
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
