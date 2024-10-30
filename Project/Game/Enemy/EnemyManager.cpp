#include"EnemyManager.h"
#include"3d/ModelManager.h"
//camera
#include"camera/GameCamera.h"
//std
#include<string>
#include<format>
#include <fstream>
//Imgui
#include<imgui.h>

void EnemyManager::Init() {
	LoadEnemyPosies("resources/EnemyParamater/enemyPositions.json");
}

//レール追加
void  EnemyManager::AddNormalEnemy(const Vector3& pos) {
	std::unique_ptr<NormalEnemy>enemy;
	enemy = std::make_unique<NormalEnemy>();
	enemy->Init("axis",".obj");
	enemy->SetPos(pos);
	controlNum_++;
	positions_.push_back(enemy->GetPos());
	enemies_.push_back(std::move(enemy));
}

//レール更新
void EnemyManager::Update() {
	ImGui::Begin("IsSaveParamater");
	if (ImGui::Button("EnemySave")) {
		SaveEnemyPoses("resources/EnemyParamater/enemyPositions.json");
		std::string message = std::format("{}.json saved.","Enemy");
		MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
	}
	ImGui::End();
	auto spotIt = positions_.begin(); // controlSpot_のイテレータ
	for (const std::unique_ptr<BaseEnemy>& rail : enemies_) {
		rail->Update(); 
		if (spotIt != positions_.end()) {
			*spotIt = rail->GetPos(); // controlSpot_に更新された座標を入れる
			++spotIt; // 次の要素へ進める
		}
	}

	//消す
	enemies_.remove_if([](const std::unique_ptr<BaseEnemy>& rail) {
		if (rail->GetIsDeath()) {
			return true;
		}
		return false;
		});
}

//レール描画
void EnemyManager::Draw(const ViewProjection&viewProjection) {
	for (const std::unique_ptr<BaseEnemy>& rail : enemies_) {
		rail->Draw(viewProjection);
	}
}


// レールの制御点をJSON形式で保存する
void EnemyManager::SaveEnemyPoses(const std::string& filename) {
	json j;
	for (const auto& spot : positions_) {
		j.push_back({ spot.x, spot.y, spot.z });
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4);  // 4はインデント幅
		file.close();
	}
}

// JSONファイルから制御点を読み込む
void EnemyManager::LoadEnemyPosies(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddNormalEnemy(pos);
		}
	}
}