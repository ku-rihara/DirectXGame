#include"EnemyManager.h"
//std
#include<string>
#include<format>
#include <fstream>
/// obj
#include"Enemy/NormalEnemy.h"
#include"Enemy/SideEnemy.h"
#include"Enemy/Boss.h"
#include"Score/Score.h"
//Imgui
#include<imgui.h>

void EnemyManager::Init() {
	LoadEnemyPosies("resources/EnemyParamater/enemyPositions.json");
	LoadSideEnemyPosies("resources/EnemyParamater/SideEnemyPositions.json");
	LoadBossPosies("resources/EnemyParamater/BossPositions.json");
}

//レール追加
void  EnemyManager::AddNormalEnemy(const Vector3& pos) {
	std::unique_ptr<NormalEnemy>enemy;
	enemy = std::make_unique<NormalEnemy>();
	enemy->Init("flyEnemyBody",".obj");
	enemy->SetPos(pos);
	controlNum_++;
	positions_.push_back(enemy->GetPos());
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::AddSideEnemy(const Vector3& pos) {
	std::unique_ptr<SideEnemy>enemy;
	enemy = std::make_unique<SideEnemy>();
	enemy->Init("sideEnemy",".obj");
	enemy->SetPos(pos);
	controlNum_++;
	sideEnemyPositions_.push_back(enemy->GetPos());
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::AddBoss(const Vector3& pos) {
	std::unique_ptr<Boss>enemy;
	enemy = std::make_unique<Boss>();
	enemy->Init("Boss", ".obj");
	enemy->SetPos(pos);
	controlNum_++;
	bossPositions_.push_back(enemy->GetPos());
	enemies_.push_back(std::move(enemy));
}

//レール更新
void EnemyManager::Update(const Vector3& traget) {

	Debug();
	
	// 位置更新のためのイテレータ
	auto spotItN = positions_.begin();
	auto spotItS = sideEnemyPositions_.begin();
	auto spotItB = bossPositions_.begin();

	// enemies_ リストを更新
	for (const auto& enemy : enemies_) {
		enemy->Update(traget);

		// NormalEnemy の位置を更新
		if (auto* normalEnemy = dynamic_cast<NormalEnemy*>(enemy.get()); normalEnemy && spotItN != positions_.end()) {
			*spotItN = normalEnemy->GetPos();
			++spotItN;
		}

		// SideEnemy の位置を更新
		if (auto* sideEnemy = dynamic_cast<SideEnemy*>(enemy.get()); sideEnemy && spotItS != sideEnemyPositions_.end()) {
			*spotItS = sideEnemy->GetPos();
			++spotItS;
		}

		// BossEnemy の位置を更新
		if (auto* bossEnemy = dynamic_cast<Boss*>(enemy.get()); bossEnemy && spotItB != bossPositions_.end()) {
			*spotItB = bossEnemy->GetPos();
			++spotItB;
		}
	}

	// 死亡した敵を enemies_ から削除
	enemies_.remove_if([&](const std::unique_ptr<BaseEnemy>& enemy) {
		if (enemy->GetIsDeath()) {
			pScore_->ScoreUp(enemy->GetScoreValue());  // スコア加算
			return true;  // リストから削除する
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

void EnemyManager::Debug() {
#ifdef _DEBUG
	ImGui::Begin("IsSaveParamater");
	if (ImGui::Button("EnemySave")) {
		SaveEnemyPoses("resources/EnemyParamater/enemyPositions.json",positions_);
		std::string message = std::format("{}.json saved.", "Enemy");
		MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
	}

	if (ImGui::Button("SideEnemySave")) {
		SaveEnemyPoses("resources/EnemyParamater/SideEnemyPositions.json", sideEnemyPositions_);
		std::string message = std::format("{}.json saved.", "Enemy");
		MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
	}

		if (ImGui::Button("BossSave")) {
			SaveEnemyPoses("resources/EnemyParamater/BossPositions.json",bossPositions_);
			std::string message = std::format("{}.json saved.", "Enemy");
			MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
		}
	
	ImGui::End();
#endif
}

// レールの制御点をJSON形式で保存する
void EnemyManager::SaveEnemyPoses(const std::string& filename, const std::vector<Vector3>& pos) {
	json j;
	for (const auto& spot : pos) {
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

// JSONファイルから制御点を読み込む
void EnemyManager::LoadSideEnemyPosies(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddSideEnemy(pos);
		}
	}
}

// JSONファイルから制御点を読み込む
void EnemyManager::LoadBossPosies(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			Vector3 pos = { spot[0], spot[1], spot[2] };
			AddBoss(pos);
		}
	}
}


void EnemyManager::SetScore(Score* score) {
	pScore_ = score;
}