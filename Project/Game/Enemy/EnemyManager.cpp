
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

	spownNum_ = 1;
	currentPhase_ = 0;
	currentTime_ = 0.0f;

	// デフォルトのフェーズ0を作成
	if (phases_.find(0) == phases_.end()) {
		Phase phase;
		phases_[0] = phase;
	}
}


///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const std::vector<Vector3>& spawnPositions) {
	for (const auto& position : spawnPositions) {//　位置

	
			std::unique_ptr<BaseEnemy> enemy;

			if (selectedEnemyType_ == "NormalEnemy") {// 通常敵
				enemy = std::make_unique<NormalEnemy>();
			}

			// 位置初期化とlistに追加
			enemy->Init(position);
			enemies_.push_back(std::move(enemy));
		}
	
}


///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {
	if (isEditorMode_) {
		return; // エディタモード中は停止
	}

	currentTime_ += Frame::DeltaTime(); //* 現在時間加算

	auto& phase = phases_[currentPhase_];//* 現在フェーズの取得

	// Waveの処理
	for (Wave& wave : phase.waves) {
		if (currentTime_ >= wave.startTime) { // Wave開始

			for (auto& spawn : wave.spawns) {
				if (currentTime_ >= spawn.spownTime) {// 次の生成時間に達した場合

					/// グループ
					for (const auto& group : spawn.enemyGroups) {
						
							std::unique_ptr<BaseEnemy> enemy;

							// 種類に応じて敵追加
							if (group.enemyType == "NormalEnemy") {
								enemy = std::make_unique<NormalEnemy>();
							}

							// 他の敵タイプを追加可能
							enemy->Init(group.position);
							enemies_.push_back(std::move(enemy));
						}
					
				}
			}
		}
	}

	for (auto it = enemies_.begin(); it != enemies_.end();) {
		(*it)->Update();// 更新
		++it;
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
	ImGui::Begin("Enemy Manager");

	// エディターモードフラグ
	ImGui::SeparatorText("EditorMode");
	ImGui::Checkbox("IsEditorMode", &isEditorMode_);

	//* フェーズ選択と Wave 編集
	ImGui::InputInt("Current Phase", &currentPhase_);
	auto& phase = phases_[currentPhase_];

	for (size_t waveIndex = 0; waveIndex < phase.waves.size(); ++waveIndex) {
		auto& wave = phase.waves[waveIndex];
		if (ImGui::CollapsingHeader(std::format("Wave {}", waveIndex).c_str())) {
			ImGui::DragFloat("Start Time", &wave.startTime, 0.1f);

			// 敵の生成パラメータについて
			for (size_t spawnIndex = 0; spawnIndex < wave.spawns.size(); ++spawnIndex) {
				auto& spawn = wave.spawns[spawnIndex];
				ImGui::PushID(int(spawnIndex));

				/// 敵グループ
				ImGui::SeparatorText("Enemy Groups:");
				for (size_t groupIndex = 0; groupIndex < spawn.enemyGroups.size(); ++groupIndex) {
					auto& group = spawn.enemyGroups[groupIndex];
					ImGui::PushID(int(groupIndex));

					// Remove Group ボタンを同じ行に配置
					ImGui::Text("Group %zu", groupIndex); // グループインデックス表示
					ImGui::SameLine();
					if (ImGui::Button("Remove Group")) {
						spawn.enemyGroups.erase(spawn.enemyGroups.begin() + groupIndex);
						ImGui::PopID();
						continue; // 削除後に他のUI要素を更新しないように
					}

					// 敵グループの設定UI
					ImGui::InputText("Type", &group.enemyType[0], group.enemyType.size());
					ImGui::DragFloat3("Position", &group.position.x, 0.1f);
					/*ImGui::DragInt("Count", (int*)&group.count, 1, 1, 100);*/

					ImGui::PopID();
				}

				// 敵グループ追加
				if (ImGui::Button("Add Enemy Group")) {
					spawn.enemyGroups.push_back({ "NormalEnemy", {},});
				}

				// 生成するまでかかる時間
				ImGui::DragFloat("Spawn Time", &spawn.spownTime, 0.1f);

				ImGui::PopID();
			}

			if (ImGui::Button("Add Spawn")) {
				wave.spawns.push_back({ {}, 2.0f });
			}
		}

		ImGui::SeparatorText("Wave");
		if (ImGui::Button(std::format("Remove Wave ##{}", waveIndex).c_str())) {
			phase.waves.erase(phase.waves.begin() + waveIndex);
		}
	}

	if (ImGui::Button("Add Wave")) {
		phase.waves.push_back({ currentTime_, {} });
	}

	/// セーブロード
	ImGui::SeparatorText("Save and Load");
	SaveAndLoad();

	ImGui::End();
}


void EnemyManager::SetPhase(int phase) {
	if (phases_.find(phase) != phases_.end()) {
		currentPhase_ = phase;
		currentTime_ = 0.0f;
	}
	else {
		ImGui::Text("Invalid phase!");
	}
}

void EnemyManager::SaveAndLoad() {
	if (ImGui::Button("Save Schedules")) {
		json scheduleJson;
		for (const auto& [phaseNum, phase] : phases_) {
			json phaseJson;
			for (const auto& wave : phase.waves) {
				json waveJson;
				waveJson["startTime"] = wave.startTime;

				for (const auto& spawn : wave.spawns) {
					json spawnJson;
					spawnJson["SpownTime"] = spawn.spownTime; 

					for (const auto& group : spawn.enemyGroups) {
						spawnJson["enemyGroups"].push_back({
							{"enemyType", group.enemyType},
							{"position", {{"x", group.position.x}, {"y", group.position.y}, {"z", group.position.z}}}
							});
					}

					waveJson["spawns"].push_back(spawnJson);
				}

				phaseJson.push_back(waveJson);
			}
			scheduleJson[std::to_string(phaseNum)] = phaseJson;
		}

		std::ofstream file(directrypath_ + filename_);
		if (file.is_open()) {
			file << scheduleJson.dump(4); // JSON をインデント付きで保存
		}
	}

	if (ImGui::Button("Load Schedules")) {
		std::ifstream file(directrypath_ + filename_);
		if (file.is_open()) {
			json scheduleJson;
			file >> scheduleJson;

			phases_.clear();
			for (auto& [phaseNum, phaseData] : scheduleJson.items()) {
				Phase phase;
				for (const auto& waveData : phaseData) {
					Wave wave;

					// startTime の型チェック
					if (waveData.contains("startTime") && waveData["startTime"].is_number()) {
						wave.startTime = waveData["startTime"];
					}

					for (const auto& spawnData : waveData["spawns"]) {
						EnemySpawn spawn;

						// spawnInterval の型チェック
						if (spawnData.contains("SpownTime") && spawnData["SpownTime"].is_number()) {
							spawn.spownTime = spawnData["SpownTime"];
						}

						for (const auto& groupData : spawnData["enemyGroups"]) {
							if (groupData.contains("enemyType") && groupData["enemyType"].is_string() &&
								groupData.contains("position") && groupData["position"].is_object() &&
								groupData["position"].contains("x") && groupData["position"]["x"].is_number() &&
								groupData["position"].contains("y") && groupData["position"]["y"].is_number() &&
								groupData["position"].contains("z") && groupData["position"]["z"].is_number()/* &&
								groupData.contains("count") && groupData["count"].is_number_integer()*/) {

								// Vector3 の座標値を安全にロード
								Vector3 position{
									groupData["position"]["x"],
									groupData["position"]["y"],
									groupData["position"]["z"]
								};

								spawn.enemyGroups.push_back({
									groupData["enemyType"],
									position
									});
							}
						}
						wave.spawns.push_back(spawn);
					}
					phase.waves.push_back(wave);
				}
				phases_[std::stoi(phaseNum)] = phase;
			}
		}
	}
}



void EnemyManager::SetEditorMode(bool isEditorMode) {
	isEditorMode_ = isEditorMode;
}
