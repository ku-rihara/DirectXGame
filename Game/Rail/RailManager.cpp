#include"RailManager.h"
#include"ModelManager.h"

void RailManager::Init() {
	
}

//レール追加
void  RailManager::AddRail(const Vector3& pos) {
	std::unique_ptr<Rail>rail;
	rail = std::make_unique<Rail>();
	rail->objct3D_.reset(Object3d::CreateModel("cube", ".obj"));
	rail->SetPos(pos);
	rails_.push_back(std::move(rail));
}

//レール更新
void RailManager::Update() {
	for (const std::unique_ptr<Rail>& rail : rails_) {
		rail->Update();
	}

	rails_.remove_if([](const std::unique_ptr<Rail>& rail) {
		if (rail->GetIsDeath()) {
			return true;
		}
		return false;
		});
}

//レール描画
void RailManager::Draw(const ViewProjection&viewProjection) {
	for (const std::unique_ptr<Rail>& rail : rails_) {
		rail->Draw(viewProjection);
	}
}