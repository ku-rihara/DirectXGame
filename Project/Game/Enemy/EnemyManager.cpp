
#include "EnemyManager.h"
#include"NormalEnemy.h"

#include "Frame/Frame.h"

#include <format>
#include <fstream>

#include <imgui.h>

EnemyManager::EnemyManager() {

}

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {

	selectedEnemyType_ = "NormalEnemy";
	spownPosition_ = {};
	spownTime_ = 0.0f;
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const Vector3& spawnPos) {
	std::unique_ptr<BaseEnemy> enemy;

	if (selectedEnemyType_ == "NormalEnemy") {
		enemy = std::make_unique<NormalEnemy>();
	}
	// 他の敵タイプもここに追加可能

	enemy->Init(spawnPos);
	enemies_.push_back(std::move(enemy));
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {
	currentTime_ += Frame::DeltaTime();
	for (auto it = spownSchedule_.begin(); it != spownSchedule_.end(); ++it) {
		if (currentTime_ >= it->first) { // 時間超えたら生成
			SpawnEnemy(it->second);
			it = spownSchedule_.erase(it);
		}
		else {
			++it; // 次の要素
		}
	}

	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		(*it)->Update();
	
		// if ((*it)->ShouldBeRemoved()) {
		//     it = enemies_.erase(it);
		// } else {
		//     ++it;
		// }
	}
}

///========================================================================================
///  描画処理
///========================================================================================
void EnemyManager::Draw(const ViewProjection& viewProjection) {
	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		(*it)->Draw(viewProjection);
	}
}

///========================================================================================
///  スプライト描画処理
///========================================================================================
void EnemyManager::SpriteDraw(const ViewProjection& viewProjection) {
	for (auto it = enemies_.begin(); it != enemies_.end(); ++it) {
		(*it)->SpriteDraw(viewProjection);
	}
}

///========================================================================================
///  ImGuiによるエディタ
///========================================================================================
void EnemyManager::ImGuiUpdate() {
#ifdef _DEBUG

	ImGui::Begin("Enemy Manager");/// begin

	/// 敵の種類選択
	if (ImGui::BeginCombo("Enemy Type", selectedEnemyType_.c_str())) {
		for (const auto& type : enemyTypes_) {
			if (ImGui::Selectable(type.c_str(), type == selectedEnemyType_)) {
				selectedEnemyType_ = type;
			}
		}
		ImGui::EndCombo();
	}

	// 敵の座標と時間を設定
	ImGui::DragFloat3("Spawn Position", &spownPosition_.x, 0.1f);
	ImGui::DragFloat("Spawn Time", &spownTime_, 0.1f);

	// スケジュール表示
	ImGui::SeparatorText("Spawn Schedule:");

	for (size_t i = 0; i < spownSchedule_.size(); ++i) {
		ImGui::Text("Time: %.2f, Position: (%.2f, %.2f, %.2f)", spownSchedule_[i].first,
			spownSchedule_[i].second.x, spownSchedule_[i].second.y, spownSchedule_[i].second.z);
		/// 敵削除
		if (ImGui::Button(std::format("Remove##{}", i).c_str())) {
			spownSchedule_.erase(spownSchedule_.begin() + i);
		}
	}

	/// セーブ
	if (ImGui::Button("Save Schedule")) {
		json schedule;
		/// push back
		for (const auto& [time, pos] : spownSchedule_) {
			schedule.push_back({ {"time",time}, {"position", { {"x", pos.x}, {"y", pos.y}, {"z", pos.z} }} });
		}

		//ファイルに書き込み
		std::ofstream file("enemy_schedule.json");
		if (file.is_open()) {
			file << schedule.dump(4);
			file.close();
		}
	}
		// ロード
		ImGui::SameLine();
		if (ImGui::Button("Load Schedule")) {
			std::ifstream file("enemy_schedule.json");
			if (file.is_open()) {
				json schedule;
				file >> schedule;
				file.close();

				spownSchedule_.clear();
				for (const auto& entry : schedule) {
					Vector3 pos = { entry["position"]["x"], entry["position"]["y"], entry["position"]["z"] };
					spownSchedule_.emplace_back(entry["time"], pos);
				}
			}
		}

		ImGui::End();


#endif // _DEBUG
	}

