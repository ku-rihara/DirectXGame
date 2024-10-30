#include"ControlPointManager.h"
#include"3d/ModelManager.h"
//camera
#include"camera/GameCamera.h"
//std
#include<string>
#include<format>
#include <fstream>
//Imgui
#include<imgui.h>

void ControlPointManager::Init() {
	
	LoadControlSpots("resources/RailParamater/controlPoint.json");
	
}

//レール追加
void  ControlPointManager::AddControlPoint(const Vector3& pos) {
	std::unique_ptr<ControlPoint>rail;
	rail = std::make_unique<ControlPoint>();
	rail->Init();
	rail->SetPos(pos);
	controlNum_++;
	controlSpots_.push_back(rail->GetPos());
	rails_.push_back(std::move(rail));
	
}

//レール更新
void ControlPointManager::Update() {
	ImGui::Begin("IsSaveParamater");
	if (ImGui::Button("Save")) {
		SaveControlSpots("resources/RailParamater/controlPoint.json");
		std::string message = std::format("{}.json saved.","Rail");
		MessageBoxA(nullptr, message.c_str(), "RailParamater", 0);
	}
	ImGui::End();
	auto spotIt = controlSpots_.begin(); // controlSpot_のイテレータ
	for (const std::unique_ptr<ControlPoint>& rail : rails_) {
		rail->Update(); 
		if (spotIt != controlSpots_.end()) {
			*spotIt = rail->GetPos(); // controlSpot_に更新された座標を入れる
			++spotIt; // 次の要素へ進める
		}
	}

	//消す
	rails_.remove_if([](const std::unique_ptr<ControlPoint>& rail) {
		if (rail->GetIsDeath()) {
			return true;
		}
		return false;
		});
}

//レール描画
void ControlPointManager::Draw(const ViewProjection&viewProjection) {
	for (const std::unique_ptr<ControlPoint>& rail : rails_) {
		rail->Draw(viewProjection);
	}
}


// レールの制御点をJSON形式で保存する
void ControlPointManager::SaveControlSpots(const std::string& filename) {
	json j;
	for (const auto& spot : controlSpots_) {
		j.push_back({ spot.x, spot.y, spot.z });
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4);  // 4はインデント幅
		file.close();
	}
}

// JSONファイルから制御点を読み込む
void ControlPointManager::LoadControlSpots(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddControlPoint(pos);
		}
	}
}