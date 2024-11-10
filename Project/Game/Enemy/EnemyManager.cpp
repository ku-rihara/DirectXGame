#include"EnemyManager.h"
//std
#include<string>
#include<format>
#include <fstream>
/// obj
#include"Enemy/NormalEnemy.h"
#include"Score/Score.h"
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
void EnemyManager::Update(const Vector3& traget) {

	Debug();

	auto spotIt = positions_.begin(); // controlSpot_のイテレータ
	for (std::list<std::unique_ptr<BaseEnemy>>::iterator enemyIter = enemies_.begin(); enemyIter != enemies_.end();) {
		(*enemyIter)->Update(traget);

		// controlSpot_の更新
		if (spotIt != positions_.end()) {
			*spotIt = (*enemyIter)->GetPos();
			++spotIt;
		}

		// 敵の死亡
		if ((*enemyIter)->GetIsDeath()) {
			pScore_->ScoreUp((*enemyIter)->GetScoreValue());/// スコア加算
			enemyIter = enemies_.erase(enemyIter);/// リスト除外
		}
		else {
			++enemyIter;
		}
	}

}

//レール描画
void EnemyManager::Draw(const ViewProjection&viewProjection) {
	for (const std::unique_ptr<BaseEnemy>& rail : enemies_) {
		rail->Draw(viewProjection);
	}
}

void EnemyManager::Debug() {
	ImGui::Begin("IsSaveParamater");
	if (ImGui::Button("EnemySave")) {
		SaveEnemyPoses("resources/EnemyParamater/enemyPositions.json");
		std::string message = std::format("{}.json saved.", "Enemy");
		MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
	}
	ImGui::End();
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

void EnemyManager::SetScore(Score* score) {
	pScore_ = score;
}