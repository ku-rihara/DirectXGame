#include"SpeedPointManager.h"
#include"ControlPoint/SlowPoint.h"
#include"ControlPoint/FastPoint.h"
#include"3d/ModelManager.h"
//camera
#include"camera/GameCamera.h"
//std
#include<string>
#include<format>
#include <fstream>
//Imgui
#include<imgui.h>

void  SpeedPointManager::Init() {

	LoadSlowSpots("resources/SpeedParamater/slowPoint.json");
	LoadFastSpots("resources/SpeedParamater/fastPoint.json");

}

//レール追加
void   SpeedPointManager::AddSlowSpeed(const Vector3& pos) {
	std::unique_ptr<SlowPoint>rail;
	rail = std::make_unique<SlowPoint>();
	rail->Init();
	rail->SetPos(pos);
	controlNum_++;
	slowSpeed_.push_back(rail->GetPos());
	rails_.push_back(std::move(rail));

}

//レール追加
void   SpeedPointManager::AddFastSpeed(const Vector3& pos) {
	std::unique_ptr<FastPoint>rail;
	rail = std::make_unique<FastPoint>();
	rail->Init();
	rail->SetPos(pos);
	controlNum_++;
	fastSpeed_.push_back(rail->GetPos());
	rails_.push_back(std::move(rail));

}

//レール更新
void  SpeedPointManager::Update() {
	Debug();
	auto spotItS = slowSpeed_.begin();
	auto spotItF = fastSpeed_.begin();

	for (const std::unique_ptr<BaseSpeedControl>& rail : rails_) {
		rail->Update();

		// SlowPointの更新処理
		if (auto* slowPoint = dynamic_cast<SlowPoint*>(rail.get()); slowPoint && spotItS != slowSpeed_.end()) {
			*spotItS = slowPoint->GetPos();
			++spotItS;
		}

		// FastPointの更新処理
		if (auto* fastPoint = dynamic_cast<FastPoint*>(rail.get()); fastPoint && spotItF != fastSpeed_.end()) {
			*spotItF = fastPoint->GetPos();
			++spotItF;
		}
	}


	//消す
	rails_.remove_if([](const std::unique_ptr<BaseSpeedControl>& rail) {
		if (rail->GetIsDeath()) {
			return true;
		}
		return false;
		});
}

void SpeedPointManager::Debug() {
	ImGui::Begin("IsSaveParamater");
	if (ImGui::Button("SlowSave")) {
		SaveControlSpots("resources/SpeedParamater/slowPoint.json");
		std::string message = std::format("{}.json saved.", "SpeedPoint");
		MessageBoxA(nullptr, message.c_str(), "SpeedParamater", 0);
	}

	if (ImGui::Button("FastSave")) {
		SaveControlSpots("resources/SpeedParamater/fastPoint.json");
		std::string message = std::format("{}.json saved.", "SpeedPoint");
		MessageBoxA(nullptr, message.c_str(), "SpeedParamater", 0);
	}
	ImGui::End();
}

//レール描画
void  SpeedPointManager::Draw(const ViewProjection& viewProjection) {
	for (const std::unique_ptr<BaseSpeedControl>& rail : rails_) {
		rail->Draw(viewProjection);
	}
}


// レールの制御点をJSON形式で保存する
void  SpeedPointManager::SaveControlSpots(const std::string& filename) {
	json j;
	for (const auto& spot : slowSpeed_) {
		j.push_back({ spot.x, spot.y, spot.z });
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4);  // 4はインデント幅
		file.close();
	}
}

// JSONファイルから制御点を読み込む
void  SpeedPointManager::LoadSlowSpots(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddSlowSpeed(pos);
		}
	}
}


// JSONファイルから制御点を読み込む
void  SpeedPointManager::LoadFastSpots(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddFastSpeed(pos);
		}
	}
}

