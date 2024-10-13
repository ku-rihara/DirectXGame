#include"RailManager.h"
#include"ModelManager.h"
//camera
#include"GameCamera.h"
//std
#include<string>

void RailManager::Init() {
	globalParameter_ = GlobalParameter::GetInstance();
	const char* groupName = "RailManager";
	// グループを追加
	globalParameter_->CreateGroup(groupName);	
	globalParameter_->SetValue(groupName, "ControlNum", controlNum_);
	for (int i = 0; i < controlNum_; i++) {
		globalParameter_->SetValue(groupName, "constrolSpot"+std::to_string(int(i)), controlSpot_[i]);
	}
	
}

//レール追加
void  RailManager::AddRail(const Vector3& pos) {
	std::unique_ptr<Rail>rail;
	rail = std::make_unique<Rail>();
	rail->objct3D_.reset(Object3d::CreateModel("cube", ".obj"));
	rail->SetPos(pos);
	controlNum_++;
	controlSpot_.push_back(rail->GetPos());
	rails_.push_back(std::move(rail));
	
}

//レール更新
void RailManager::Update() {
	
	auto spotIt = controlSpot_.begin(); // controlSpot_のイテレータ
	for (const std::unique_ptr<Rail>& rail : rails_) {
		rail->Update(); 
		if (spotIt != controlSpot_.end()) {
			*spotIt = rail->GetPos(); // controlSpot_に更新された座標を入れる
			++spotIt; // 次の要素へ進める
		}
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

void RailManager::AdaptationControlSpot() {
	//if (isAdaption_) {
	//	//制御点の座標をカメラに適応
	//	controlSpot_.clear();
	//	for (const std::unique_ptr<Rail>& rail : rails_) {
	//		controlSpot_.push_back(rail->GetPos());
	//	}
	//	isAdaption_ = false;
	//}

}

void RailManager::ApplyGlobalParameter() {

	GlobalParameter* globalParameter = GlobalParameter::GetInstance();
	const char* groupName = "RailManager";
	controlNum_ = globalParameter->GetValue<int>(groupName, "ControlNum");
	for (int i = 0; i < controlNum_; i++) {
		controlSpot_[i] = globalParameter->GetValue<Vector3>(groupName, "constrolSpot" + std::to_string(int(i)));
	}

}