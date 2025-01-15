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

	ctest1_ = std::make_unique<CollisionTest1>();
	ctest2_ = std::make_unique<CollisionTest2>();
	ctest3_ = std::make_unique<CollisionTest3>();

	emitter_.reset(ParticleEmitter::CreateParticle("punchEffect", "cube", ".obj", 200));


	ctest1_->Init();
	ctest2_->Init();
	ctest3_->Init();
}

void TitleScene::Update() {

	ctest1_->Update();
	ctest2_->Update();
	ctest3_->Update();

	emitter_->Update();
	emitter_->EditorUpdate();
	emitter_->Emit();

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
	emitter_->DebugDraw(viewProjection_);
	emitter_->RailDraw(viewProjection_);
	ParticleManager::GetInstance()->Draw(viewProjection_);
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
